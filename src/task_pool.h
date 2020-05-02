
#ifndef _GS_TASK_POOL_H_
#define _GS_TASK_POOL_H_ 

#include "platform.h"
#include "task.h"
#include "mutex.h"
#include "queue.h"
#include "memory.h"

struct gs_task_context_t;

/**
 * @brief Class use to store the different tasks that threads in the thread pool
 * can consume and process. The task pool has different independent queues.
 * Typically, there will be one queue per thread, but we are not restricted to
 * this.
 */
typedef struct gs_task_pool_t 
{
  /**
   * @brief Array of task queues. There is one per thread.
   */
  gs_queue_t*                m_queues;
  

  /**
   * \brief Mutexes used to synchronize the access to the queues
   */
  gs_mutex_t*                m_mutexes;    

  /**
   * @brief The number of queues
   */
  uint32_t                    m_num_queues;

  /**
   * \brief The allocator to use
   */
  gs_mem_allocator_t*        p_allocator;

} gs_task_pool_t;

/**
 * \brief Initialize the task pool
 *
 * \param task_pool The task pool to initialize
 */
void
gs_task_pool_init(gs_task_pool_t* task_pool,
               uint32_t num_queues, 
               gs_mem_allocator_t* allocator);

/**
 * \brief Releases the task pool
 *
 * \param task_pool The task pool to release
 */
void
gs_task_pool_release(gs_task_pool_t* task_pool);


/**
 * @brief Adds a task to execute in the given queue
 *
 * @param queueId
 * @param task_context
 */
void
gs_task_pool_add_task(gs_task_pool_t* task_pool,
                      uint32_t queue_id,
                      struct gs_task_context_t* task_context);


/**
 * @brief Gets the next task to process
 *
 * @param queueId The thread to get the task for 
 *
 * @return Pointer to the task if this exists. nullptr otherwise. 
 */
struct gs_task_context_t*
gs_task_pool_get_next(gs_task_pool_t* task_pool,
                      uint32_t queue_id);

/**
 * \brief Counts the  number of tasks in the pool
 *
 * \param task_pool The task pool to count to tasks for
 * \param queue_id The queue to count the tasks for
 *
 * \return Returns the number of tasks in the pool
 */
uint32_t
gs_task_pool_count(gs_task_pool_t* task_pool,
                   uint32_t queue_id);

#endif
