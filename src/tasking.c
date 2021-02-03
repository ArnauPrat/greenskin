
#include "atomic_counter.h"
#include "boost_context/fcontext.h"
#include "cond_var.h"
#include "log.h"
#include "memory.h"
#include "mutex.h"
#include "platform.h"
#include "queue.h"
#include "task_pool.h"
#include "tasking.h"
#include "thread.h"
#include "trace.h"

#include <sched.h>
#include <string.h>


/**
 * \brief Stores the context of an execution
 */
typedef struct gs_execution_context_t
{
  char* p_stack;
  fcontext_t m_context;
} gs_execution_context_t;

/**
 * @brief  Structure used to represent a task to be executed by the thread pool
 */
typedef struct gs_task_context_t 
{

  /**
   * @brief Task encapsulated within this task context
   */
  gs_task_t                m_task;

  /**
   * @brief Synchronization counter used to synchronize this task (optional)
   */
  gs_atomic_counter_t*     p_syn_counter;


  /**
   * \brief Yield function used to check if this task is ready for running after
   * a yield (optional)
   */
  gs_yield_func_t          p_yield_func;


  /**
   * \brief Data passed to yield function
   */
  void*                 p_yield_data;

  /**
   * @bried The execution context of this task
   */
  gs_execution_context_t*  p_context;

  /**
   * @brief Whether the task is finished or not
   */
  bool                  m_finished;

  /**
   * @brief A pointer to the parent of the task in the task dependency graph
   */
  struct gs_task_context_t*       p_parent;

  /**
   * \brief Task name
   */
  char                  m_name[GS_TASKING_MAX_NAME_LEN];

  /**
   * @brief Task info
   */
  char                  m_info[GS_TASKING_MAX_INFO_LEN];


} gs_task_context_t;  

/**
 * \brief Queue with available task contexts
 */
static gs_queue_t        m_execution_context_queue;

/**
 * \brief Mutex to synchronize access to task_context_queue
 */
static gs_mutex_t         m_execution_context_queue_mutex;

/**
 * \brief Queue with available task contexts
 */
static gs_queue_t         m_task_context_queue;

/**
 * \brief Mutex to synchronize access to task_context_queue
 */
static gs_mutex_t        m_task_context_queue_mutex;

/**
 * \brief stores if the threadpool is initialized
 */
static bool                         m_initialized = false;

/**
 * @brief Atomic booleans to control threads running
 */
static gs_atomic_counter_t*         m_is_running = NULL;

/**
 * @brief The number of threads in the thread pool
 */
static uint32_t                     m_num_threads = 0;

/**
 * @brief Vector of running threads objects
 */
static gs_thread_t*                 m_threads = NULL;

/**
 * @brief Pointer to the task pool with the tasks pending to start 
 */
static gs_task_pool_t               m_to_start_task_pool;

/**
 * @brief Vectors holding the running tasks of the thread.
 */
static gs_task_pool_t               m_running_task_pool;

/**
 * @brief Array of Contexts used to store the main context of each thread to
 * fall back to it when yielding a task
 */
static gs_execution_context_t**      m_worker_contexts = NULL;

/**
 * @brief Mutexes used for the condition variables for notifying sleeping
 * threads that more work is ready
 */
static gs_mutex_t*                  m_condvar_mutexes = NULL;

/**
 * @brief Condition variables sed to notify sleeping threads that more work is
 * ready
 */
static gs_cond_var_t*               m_condvars = NULL;

/**
 * @brief Pointer to the thread local currently running task 
 */
static GS_THREAD gs_task_context_t*    p_current_running_task = NULL;

/**
 * @brief Thread local variable to store the id of the current thread
 */
static GS_THREAD uint32_t             m_current_thread_id = INVALID_THREAD_ID;


/**
 * \brief The pool allocator for the task stack frames
 */
static gs_pool_alloc_t                m_stack_frame_allocator;

/**
 * \brief The pool allocator for the execution contexts
 */
static gs_pool_alloc_t                m_econtext_allocator;

/**
 * \brief The pool allocator for the execution contexts
 */
static gs_pool_alloc_t                m_tcontext_allocator;

/**
 * \brief Stack allocator to allocat the static data upon initialization
 */
static gs_stack_alloc_t               m_tasking_allocator;

/**
 * \brief The worker thread tasks
 */
static gs_task_t*                     m_thread_tasks = NULL;

void
gs_execution_context_init(gs_execution_context_t* exec_context)
{
  memset(exec_context, 0, sizeof(gs_execution_context_t));
  GS_ASSERT(gs_os_page_size() == GS_SYSTEM_SMALL_PAGE_SIZE && "Stack size must be equal to the os page size");
  void* ptr = gs_pool_alloc_alloc(&m_stack_frame_allocator, 
                                  GS_SYSTEM_SMALL_PAGE_SIZE,
                                  GS_SYSTEM_SMALL_PAGE_SIZE + 2*GS_SYSTEM_SMALL_PAGE_SIZE, 
                                  GS_NO_HINT);

  //int result = posix_memalign(&ptr, getpagesize(), m_stack_size + 2*getpagesize());
  GS_ASSERT( ptr != NULL && "Failed aligned memory allocation");
  // memory guard
  bool result = gs_os_protect_mem(ptr,GS_SYSTEM_SMALL_PAGE_SIZE);
  GS_ASSERT(result && "Couldn't set page protection");
  result = gs_os_protect_mem((char*)ptr + GS_SYSTEM_SMALL_PAGE_SIZE + GS_SYSTEM_SMALL_PAGE_SIZE, 
                             GS_SYSTEM_SMALL_PAGE_SIZE);
  GS_ASSERT(result && "Couldn't set page protection");
  exec_context->p_stack = (char*)ptr;
}

void
gs_execution_context_release(gs_execution_context_t* exec_context)
{
  bool result = gs_os_unprotect_mem(exec_context->p_stack, GS_SYSTEM_SMALL_PAGE_SIZE);
  GS_ASSERT(result && "Couldn't remove page protection");
  result = gs_os_unprotect_mem(exec_context->p_stack + GS_SYSTEM_SMALL_PAGE_SIZE + GS_SYSTEM_SMALL_PAGE_SIZE, 
                               GS_SYSTEM_SMALL_PAGE_SIZE);
  GS_ASSERT(result && "Couldn't remove page protection");
  gs_pool_alloc_free(&m_stack_frame_allocator, exec_context->p_stack);
}

void
gs_execution_context_reset(gs_execution_context_t* exec_context)
{
  exec_context->m_context = NULL;
}

/**
 * \brief Inserts a timing event into the correponding queue
 *
 * \param queueid The queueid
 * \param event_type The event_type
 */
static void
insert_timing_event(uint32_t queue_id, 
                    gs_trace_event_type_t event_type, 
                    gs_task_category_t category, 
                    gs_task_context_t* task_context)
{
  GS_TRACE_RECORD(queue_id, 
                  event_type,
                  category,
                  task_context->m_name, 
                  task_context->m_info);
}

/**
 * @brief Finalizes the current running task and releases its resources
 */
static void 
finalize_current_running_task() {
  GS_ASSERT(gs_tasking_get_current_thread_id() < m_num_threads && "Invalid thead id");

  if(p_current_running_task->m_finished) 
  {
    // insert event
    insert_timing_event(gs_tasking_get_current_thread_id(), 
                        E_TRACE_TASK_STOP, 
                        p_current_running_task->m_task.m_category,
                        p_current_running_task);

    if(p_current_running_task->p_syn_counter != NULL) 
    {
      gs_atomic_counter_fetch_decrement(p_current_running_task->p_syn_counter);
    }
    gs_mutex_lock(&m_execution_context_queue_mutex);
    gs_queue_push(&m_execution_context_queue, p_current_running_task->p_context);
    gs_mutex_unlock(&m_execution_context_queue_mutex);
    p_current_running_task->p_context = NULL;

    gs_mutex_lock(&m_task_context_queue_mutex);
    gs_queue_push(&m_task_context_queue, p_current_running_task);
    gs_mutex_unlock(&m_task_context_queue_mutex);
  }
  else 
  {
    gs_task_pool_add_task(&m_running_task_pool, 
                       m_current_thread_id, 
                       p_current_running_task);
  }
  p_current_running_task = NULL;
}

void
fiber_function(void* arg)
{
  GS_ASSERT(gs_tasking_get_current_thread_id() < m_num_threads && "Invalid thead id");
  
  gs_task_context_t* task_context = (gs_task_context_t*)arg;
  task_context->m_task.p_fp(task_context->m_task.p_args);
  task_context->m_finished = true;
  jump_fcontext(&task_context->p_context->m_context, 
                               m_worker_contexts[gs_tasking_get_current_thread_id()]->m_context, 
                               task_context->m_task.p_args);

}

/**
 * @brief Start the execution of the given task. A lambda function is passed
 * to the callcc method, which takes as input the current execution context.
 * This current execution context is stored int he m_worker_contexts array
 * for the current thread. Then the task is executed. 
 *
 * @param task The task to execute
 */
static void 
gs_start_task(gs_task_context_t* task_context) 
{
  GS_ASSERT(gs_tasking_get_current_thread_id() < m_num_threads && "Invalid thead id");

  // find free fiber
  gs_mutex_lock(&m_execution_context_queue_mutex);
  gs_execution_context_t* ctx = gs_queue_pop(&m_execution_context_queue); 
  gs_mutex_unlock(&m_execution_context_queue_mutex);

  if(ctx == NULL)
  {
    GS_LOG_ERROR(E_TASKING_MAX_ECONTEXTS_ERROR,"");
  }

  // we reset it in case it is a reused context
  gs_execution_context_reset(ctx);

  // initialize fiber task function
  task_context->p_context = ctx;

  ctx->m_context = make_fcontext(ctx->p_stack + GS_SYSTEM_SMALL_PAGE_SIZE + GS_SYSTEM_SMALL_PAGE_SIZE, 
                                                GS_SYSTEM_SMALL_PAGE_SIZE, 
                                                fiber_function);
  // set the task as the current running task
  p_current_running_task = task_context; 

  // insert event
  insert_timing_event(gs_tasking_get_current_thread_id(), 
                      E_TRACE_TASK_START, 
                      p_current_running_task->m_task.m_category,
                      p_current_running_task);

  // jump to fiber
  jump_fcontext(&(m_worker_contexts[gs_tasking_get_current_thread_id()]->m_context), 
                ctx->m_context, 
                task_context);
  finalize_current_running_task();
}


/**
 * @brief Resumes the given running task 
 *
 * @param runningTask The task to resume
 */
static void 
resume_task(gs_task_context_t* runningTask) 
{
  GS_ASSERT(gs_tasking_get_current_thread_id() < m_num_threads && "Invalid thead id");

  p_current_running_task = runningTask;

  // insert event
  insert_timing_event(gs_tasking_get_current_thread_id(), 
                      E_TRACE_TASK_RESUME, 
                      runningTask->m_task.m_category,
                      p_current_running_task);

  // call jump to fiber to resume
  jump_fcontext(&(m_worker_contexts[gs_tasking_get_current_thread_id()]->m_context), 
                runningTask->p_context->m_context, 
                runningTask);
  finalize_current_running_task();
}

/**
 * @brief The main function that each thread runs
 *
 * @param threadId The id of the thread that runs the function
 */
static void 
thread_function(void* threadId) 
{
  m_current_thread_id = (uint32_t)(uint64_t)threadId;
  gs_atomic_counter_set(&m_is_running[m_current_thread_id], 1);
  while(gs_atomic_counter_get(&m_is_running[m_current_thread_id]))
  {
    gs_task_context_t* task_context = gs_task_pool_get_next(&m_to_start_task_pool, 
                                                            m_current_thread_id);
    if(task_context != NULL) 
    {
      gs_start_task(task_context);
    } 
    else 
    {
      task_context = gs_task_pool_get_next(&m_running_task_pool, m_current_thread_id);
      bool can_run = task_context && 
                    (task_context->p_yield_func == NULL || task_context->p_yield_func(task_context->p_yield_data));
      if (can_run)
      {
        resume_task(task_context);
      } 
      else 
      {
        if(task_context != NULL)
        {
          gs_task_pool_add_task(&m_running_task_pool, m_current_thread_id, task_context);
          task_context = NULL;
        }
        //Wait until notified
        {
          gs_mutex_lock(&m_condvar_mutexes[m_current_thread_id]);
          if(gs_task_pool_count(&m_running_task_pool, m_current_thread_id) == 0 &&
             gs_task_pool_count(&m_to_start_task_pool, m_current_thread_id) == 0 && 
             gs_atomic_counter_get(&m_is_running[m_current_thread_id]) != 0)
          {
            gs_cond_var_wait(&m_condvars[m_current_thread_id], &m_condvar_mutexes[m_current_thread_id]);
          }
        }
      }
    }
  }
}

void 
gs_tasking_init(uint32_t numThreads) 
{

  m_num_threads          = numThreads;

  if(m_num_threads > 0) 
  {

    // Initializing allocators
    gs_pool_alloc_init(&m_stack_frame_allocator, 
                       GS_SYSTEM_SMALL_PAGE_SIZE, 
                       3*GS_SYSTEM_SMALL_PAGE_SIZE,
                       GS_SYSTEM_LARGE_PAGE_SIZE,
                       &system_allocator.m_super);

    gs_pool_alloc_init(&m_econtext_allocator, 
                       64, 
                       sizeof(gs_execution_context_t), 
                       GS_SYSTEM_SMALL_PAGE_SIZE, 
                       &system_allocator.m_super);

    gs_pool_alloc_init(&m_tcontext_allocator, 
                       64, 
                       sizeof(gs_task_context_t), 
                       GS_SYSTEM_SMALL_PAGE_SIZE, 
                       &system_allocator.m_super);

    gs_stack_alloc_init(&m_tasking_allocator, 
                        GS_SYSTEM_LARGE_PAGE_SIZE, 
                        &system_allocator.m_super);

    // initializing resources that depend on numThreads
    gs_task_pool_init(&m_to_start_task_pool, numThreads, &m_tasking_allocator.m_super);
    gs_task_pool_init(&m_running_task_pool, numThreads, &m_tasking_allocator.m_super);
    m_worker_contexts  = (gs_execution_context_t**)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                                        64, 
                                                                        sizeof(gs_execution_context_t*)*numThreads, 
                                                                        GS_NO_HINT);
    for(uint32_t i = 0; i < m_num_threads; ++i)
    {
      m_worker_contexts[i] = gs_pool_alloc_alloc(&m_econtext_allocator, 
                                                 64, 
                                                 sizeof(gs_execution_context_t), 
                                                 GS_NO_HINT);
      gs_execution_context_init(m_worker_contexts[i]);
    }

    m_is_running          = (gs_atomic_counter_t*)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                                       64, 
                                                                       sizeof(gs_atomic_counter_t)*m_num_threads, 
                                                                       GS_NO_HINT);
    for(uint32_t i = 0; i < m_num_threads; ++i)
    {
      gs_atomic_counter_set(&m_is_running[i], 0);
    }

    m_threads             = (gs_thread_t*)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                               64, 
                                                               sizeof(gs_thread_t)*m_num_threads, 
                                                               GS_NO_HINT);

    for(uint32_t i = 0; i < m_num_threads; ++i)
    {
      m_threads[i] = (gs_thread_t){0};
    }

    m_condvar_mutexes     = (gs_mutex_t*)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                              64, 
                                                              sizeof(gs_mutex_t)*m_num_threads, 
                                                              GS_NO_HINT);
    for(uint32_t i = 0; i < m_num_threads; ++i)
    {
      gs_mutex_init(&m_condvar_mutexes[i]);
    }

    m_condvars            = (gs_cond_var_t*)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                                 64, 
                                                                 sizeof(gs_cond_var_t)*m_num_threads, 
                                                                 GS_NO_HINT);

    for(uint32_t i = 0; i < m_num_threads; ++i)
    {
      gs_cond_var_init(&m_condvars[i]);
    }

    m_thread_tasks     = (gs_task_t*)gs_stack_alloc_alloc(&m_tasking_allocator, 
                                                          64, 
                                                          sizeof(gs_task_t)*m_num_threads, 
                                                          GS_NO_HINT);

    // Initializing Execution and Task Context resources
    gs_queue_init(&m_execution_context_queue, GS_TASKING_MAX_EXEC_CONTEXTS);
    gs_mutex_init(&m_execution_context_queue_mutex);
    for(uint32_t i = 0; i < GS_TASKING_MAX_EXEC_CONTEXTS; ++i)
    {
      gs_execution_context_t* econtext = (gs_execution_context_t*)gs_pool_alloc_alloc(&m_econtext_allocator, 
                                                                                      64, 
                                                                                      sizeof(gs_execution_context_t), 
                                                                                      GS_NO_HINT);
      gs_execution_context_init(econtext);
      gs_queue_push(&m_execution_context_queue, econtext);
    }



    gs_queue_init(&m_task_context_queue, GS_TASKING_MAX_TASK_CONTEXTS);
    gs_mutex_init(&m_task_context_queue_mutex);
    for(uint32_t i = 0; i < GS_TASKING_MAX_EXEC_CONTEXTS; ++i)
    {
      gs_task_context_t* tcontext = (gs_task_context_t*)gs_pool_alloc_alloc(&m_tcontext_allocator, 
                                                             64, 
                                                             sizeof(gs_task_context_t), 
                                                             GS_NO_HINT);
      memset(tcontext,0,sizeof(gs_task_context_t));
      gs_queue_push(&m_task_context_queue, tcontext);
    }


    // Setting thread affinity
    for(uint32_t i = 0; i < m_num_threads; ++i) 
    {
      m_thread_tasks[i] = (gs_task_t){.p_fp = thread_function, 
                                      .p_args = (void*)(uint64_t)i}; // this is a trick, we use the pointer to store the thread id
      gs_thread_start(&m_threads[i], &m_thread_tasks[i]);
      if(!gs_thread_set_affinity(&m_threads[i], i))
      {
        GS_LOG_WARNING("Thread affinity for thread %d could not be set", i);
      }
    }

    if(!gs_thread_set_main_affinity(m_num_threads))
    {
      GS_LOG_WARNING("Thread affinity for main thread could not be set");
    }
  }
  m_initialized = true;
}

void 
gs_tasking_release(void) 
{
  GS_ASSERT(m_initialized && "ThreadPool is not initialized");

  if(m_num_threads > 0) 
  {

    // Telling threads to stop
    for(uint32_t i = 0; i < m_num_threads; ++i) 
    {
      while(gs_atomic_counter_get(&m_is_running[i]) != 1)
      {
        // thread not running yet
      }
      gs_atomic_counter_set(&m_is_running[i], 0);
    }

    // Notify threads to continue
    for(uint32_t i = 0; i < m_num_threads; ++i) 
    {
      gs_cond_var_notify_one(&m_condvars[i]);
    }

    // Waiting threads to stop
    for(uint32_t i = 0; i < m_num_threads; ++i) 
    {
      gs_thread_join(&m_threads[i]);
    }

    for(uint32_t i = 0; i < m_num_threads; ++i) {
      gs_cond_var_release(&m_condvars[i]);
      gs_mutex_release(&m_condvar_mutexes[i]);
    }

    gs_stack_alloc_free(&m_tasking_allocator, m_condvars);
    gs_stack_alloc_free(&m_tasking_allocator, m_condvar_mutexes);
    gs_stack_alloc_free(&m_tasking_allocator, m_is_running);

    for (uint32_t i = 0; i < m_num_threads; ++i) 
    {
      gs_execution_context_release(m_worker_contexts[i]);
      gs_pool_alloc_free(&m_econtext_allocator, 
                         m_worker_contexts[i]);
    }
    gs_stack_alloc_free(&m_tasking_allocator, m_worker_contexts);

    gs_task_pool_release(&m_running_task_pool);
    gs_task_pool_release(&m_to_start_task_pool);

    gs_execution_context_t* execution_context = NULL;
    while((execution_context = gs_queue_pop(&m_execution_context_queue)) != NULL)
    {
      gs_execution_context_release(execution_context);
      gs_pool_alloc_free(&m_econtext_allocator, 
                         execution_context);
    }
    gs_queue_release(&m_execution_context_queue);
    gs_mutex_release(&m_execution_context_queue_mutex);

    gs_task_context_t* task_context = NULL;
    while((task_context = gs_queue_pop(&m_task_context_queue)) != NULL)
    {
      gs_pool_alloc_free(&m_tcontext_allocator, 
                         task_context);
    }
    gs_queue_release(&m_task_context_queue);
    gs_mutex_release(&m_task_context_queue_mutex);
    gs_pool_alloc_release(&m_stack_frame_allocator);
    gs_pool_alloc_release(&m_econtext_allocator);
    gs_pool_alloc_release(&m_tcontext_allocator);
    gs_stack_alloc_release(&m_tasking_allocator);
  }
  m_initialized = false;
}

void 
gs_tasking_execute_task_async(uint32_t queueId, 
                              gs_task_t* task, 
                              gs_atomic_counter_t* counter,
                              const char* name, 
                              const char* info) 
{
  GS_ASSERT(m_initialized && "ThreadPool is not initialized");
  GS_ASSERT(m_num_threads > 0 && "Number of threads in the threadpool must be > 0");
  
  gs_mutex_lock(&m_task_context_queue_mutex);
  gs_task_context_t* task_context = gs_queue_pop(&m_task_context_queue);
  gs_mutex_unlock(&m_task_context_queue_mutex);

  if(task_context == NULL)
  {
    GS_LOG_ERROR(E_TASKING_MAX_TCONTEXTS_ERROR, "");
  }
  memset(task_context, 0, sizeof(gs_task_context_t));

  task_context->m_task = *task;
  task_context->p_syn_counter = counter;
  task_context->m_name[0] = '\0';
  if(name != NULL)
  {
    strncpy(task_context->m_name, name, GS_TASKING_MAX_NAME_LEN-1);
    if(strlen(info) >= GS_TASKING_MAX_NAME_LEN)
    {
      task_context->m_name[GS_TASKING_MAX_NAME_LEN-1] = '\0';
    }
  }
  task_context->m_info[0] = '\0';
  if(info != NULL)
  {
    strncpy(task_context->m_info, info, GS_TASKING_MAX_INFO_LEN);
    if(strlen(info) >= GS_TASKING_MAX_INFO_LEN)
    {
      task_context->m_info[GS_TASKING_MAX_INFO_LEN-1] = '\0';
    }
  }

  if(task_context->p_syn_counter != NULL) 
  {
    gs_atomic_counter_fetch_increment(task_context->p_syn_counter);
  }

  if(gs_tasking_get_current_thread_id() != INVALID_THREAD_ID ) 
  {
    task_context->p_parent = p_current_running_task;
  } 
  else 
  {
    task_context->p_parent = NULL;
  }

  gs_task_pool_add_task(&m_to_start_task_pool,
                     queueId, 
                     task_context);

  gs_mutex_lock(&m_condvar_mutexes[queueId]);
  gs_cond_var_notify_all(&m_condvars[queueId]);
} 

void 
gs_tasking_execute_task_sync(uint32_t queueId, 
                  gs_task_t* task, 
                  gs_atomic_counter_t* counter, 
                  const char* name, 
                  const char* info) 
{
  GS_ASSERT(m_initialized && "ThreadPool is not initialized");
  GS_ASSERT(m_num_threads > 0 && "Number of threads in the threadpool must be > 0");
  gs_tasking_execute_task_async(queueId, task, counter, name, info);
  gs_atomic_counter_join(counter);
}

uint32_t 
gs_tasking_get_current_thread_id(void) 
{
  GS_ASSERT(((m_current_thread_id >= 0 && m_current_thread_id < m_num_threads) || m_current_thread_id == INVALID_THREAD_ID) && "Invalid thread id");
  return m_current_thread_id;
}

void 
gs_tasking_yield(void) 
{
  gs_tasking_yield_func(NULL, NULL);
}

bool
counter_yield_func(void* counter_ptr)
{
  gs_atomic_counter_t* sync_counter = (gs_atomic_counter_t*)counter_ptr;
  int32_t value = gs_atomic_counter_get(sync_counter);
  return value == 0;
}

void
gs_tasking_yield_counter(gs_atomic_counter_t* sync_counter)
{
  gs_tasking_yield_func(counter_yield_func, sync_counter);
}

void 
gs_tasking_yield_func(gs_yield_func_t yield_func, void* yield_data) 
{
  GS_ASSERT(m_initialized && "ThreadPool is not initialized");
  GS_ASSERT(m_num_threads > 0 && "Number of threads in the threadpool must be > 0");
  GS_ASSERT(m_current_thread_id != INVALID_THREAD_ID);

  p_current_running_task->p_yield_func = yield_func;
  p_current_running_task->p_yield_data = yield_data;

  insert_timing_event(gs_tasking_get_current_thread_id(), 
                      E_TRACE_TASK_YIELD, 
                      p_current_running_task->m_task.m_category,
                      p_current_running_task);

  jump_fcontext(&(p_current_running_task->p_context->m_context), m_worker_contexts[m_current_thread_id]->m_context, NULL);
}

uint32_t
gs_tasking_get_num_threads() 
{
  GS_ASSERT(m_initialized && "ThreadPool is not initialized");
  return m_num_threads;
}
