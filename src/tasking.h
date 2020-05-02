
#ifndef _GS_THREAD_POOL_H_
#define _GS_THREAD_POOL_H_

#include "platform.h"
#include "trace.h"
#include "task.h"
#include "atomic_counter.h"

struct atomic_counter_t;

#define INVALID_THREAD_ID 0xffffffff
#define GS_MAX_INFO_STRING_LENGTH _TNA_TRACE_MAX_INFO_STRING_LENGTH
#define GS_MAX_NAME_STRING_LENGTH _TNA_TRACE_MAX_NAME_STRING_LENGTH

typedef  bool (*gs_yield_func_t)(void*);

/**
 * \brief Initializes and starts the thread pool, and creates the task queues. 
 * Currently, each thred has its own queue id, which equals the id of the thread. 
 */
void 
gs_tasking_init(uint32_t num_threads);

/**
 * \brief Stops the thread pool
 */
void 
gs_tasking_release(void);

/**
 * \brief Sends the given task for execution at the given thread
 *
 * \param task The task to run
 * \param queueId The queueId to add the task to
 * \param counter A pointer to the atomic_counter_t that will be used for
 * synchronization
 * \param info String unsed for debugging/dev 
 */
void 
gs_tasking_execute_task_async(uint32_t queueId, 
                              gs_task_t* task,
                              gs_atomic_counter_t* counter,
                              const char* name,
                              const char* info);

/**
 * \brief Sends the given task for execution at the given thread, and blocks
 * until it finishes
 *
 * \param task The task to run
 * \param queueId The queueId to add the task to
 * \param counter A pointer to the atomic_counter_t that will be used for
 * synchronization
 * \param info String unsed for debugging/dev 
 */
void 
gs_tasking_execute_task_sync(uint32_t queueId, 
                             gs_task_t* task, 
                             gs_atomic_counter_t* counter, 
                             const char* name,
                             const char* info);

/**
 * \brief Gets the current thread id
 *
 * \return  The id of the thread currently running
 */
uint32_t 
gs_tasking_get_current_thread_id(void);

/**
 * \brief Yields the current task and returns execution path to the thread pool
 * to pick another task
 */
void 
gs_tasking_yield(void);

void
gs_tasking_yield_func(gs_yield_func_t yield_func, void* data);

void
gs_tasking_yield_counter(gs_atomic_counter_t* sync_counter);

/**
 * @brief Gets the number of threads available in the buffer pool
 *
 * @return Returns the number of available threads
 */
uint32_t
gs_tasking_get_num_threads(void);


#endif
