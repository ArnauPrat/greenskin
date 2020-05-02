
#include "task_pool.h"
#include "queue.h"
#include "memory.h"
#include "mutex.h"
#include "log.h"

void
gs_task_pool_init(gs_task_pool_t* task_pool,
                  uint32_t num_qeues, 
                  gs_mem_allocator_t* allocator)
{
  task_pool->p_allocator = allocator;
  if(allocator == NULL)
  {
    task_pool->p_allocator = gs_get_global_allocator();
  }

  task_pool->m_num_queues = num_qeues;
  task_pool->m_queues = (gs_queue_t*)gs_alloc(task_pool->p_allocator, 
                                              64, 
                                              sizeof(gs_queue_t)*task_pool->m_num_queues, 
                                              GS_NO_HINT);
  for(uint32_t i = 0; i < task_pool->m_num_queues; ++i) 
  {
    gs_queue_init(&task_pool->m_queues[i], 4096);
  }

  task_pool->m_mutexes = (gs_mutex_t*) gs_alloc(task_pool->p_allocator, 
                                                64, 
                                                sizeof(gs_mutex_t)*task_pool->m_num_queues, 
                                                GS_NO_HINT);

  for(uint32_t i = 0; i < task_pool->m_num_queues; ++i) 
  {
    gs_mutex_init(&task_pool->m_mutexes[i]);
  }
}

void
gs_task_pool_release(gs_task_pool_t* task_pool) 
{
  if(task_pool->m_queues)
  {
    for(uint32_t i = 0; i < task_pool->m_num_queues; ++i) 
    {
      gs_queue_release(&task_pool->m_queues[i]);
    }
    gs_free(task_pool->p_allocator, task_pool->m_queues);
    task_pool->m_queues = NULL;
  }

  if(task_pool->m_mutexes)
  {
    for(uint32_t i = 0; i < task_pool->m_num_queues; ++i) 
    {
      gs_mutex_release(&task_pool->m_mutexes[i]);
    }
    gs_free(task_pool->p_allocator, task_pool->m_mutexes);
    task_pool->m_mutexes = NULL;
  }
}

struct gs_task_context_t* 
gs_task_pool_get_next(gs_task_pool_t* task_pool,
                   uint32_t queueId) 
{
  GS_ASSERT(queueId < task_pool->m_num_queues && queueId >= 0 && "Invalid thread queue id");
  gs_mutex_lock(&task_pool->m_mutexes[queueId]);
  struct gs_task_context_t* task = gs_queue_pop(&task_pool->m_queues[queueId]);
  gs_mutex_unlock(&task_pool->m_mutexes[queueId]);
  return task;
}

uint32_t 
gs_task_pool_count(gs_task_pool_t* task_pool,
                uint32_t queueId) 
{
  GS_ASSERT(queueId < task_pool->m_num_queues && queueId >= 0 && "Invalid thread queue id");
  gs_mutex_lock(&task_pool->m_mutexes[queueId]);
  uint32_t count = gs_queue_count(&task_pool->m_queues[queueId]);
  gs_mutex_unlock(&task_pool->m_mutexes[queueId]);
  return count;
}

void 
gs_task_pool_add_task(gs_task_pool_t* task_pool,
                   uint32_t queueId, 
                   struct gs_task_context_t* task) 
{
  GS_ASSERT(queueId < task_pool->m_num_queues && queueId >= 0 && "Invalid thread queue id");
  gs_mutex_lock(&task_pool->m_mutexes[queueId]);
  while(!gs_queue_push(&task_pool->m_queues[queueId], task))
  {
    GS_LOG_WARNING("Queue is filled up. Try incresing its current capacity: %u", task_pool->m_queues[queueId].m_capacity);
  };
  gs_mutex_unlock(&task_pool->m_mutexes[queueId]);
}

