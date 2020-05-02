
#include "queue.h"
#include "memory.h"

#include <string.h>
#include <stdlib.h>


void
gs_queue_init(gs_queue_t* queue, 
              uint32_t initial_capacity)
{
  queue->m_num_elements = 0;
  queue->m_capacity = initial_capacity;
  queue->m_start = 0;
  queue->m_end = 0;
  gs_mem_allocator_t* allocator = gs_get_global_allocator();
  queue->p_elements = (void**)gs_alloc(allocator, 
                                       64, 
                                       sizeof(void*)*queue->m_capacity, 
                                       GS_NO_HINT);
}

void
gs_queue_release(gs_queue_t* queue)
{
  gs_mem_allocator_t* allocator = gs_get_global_allocator();
  gs_free(allocator, queue->p_elements);
  queue->m_num_elements = 0;
  queue->m_capacity = 0;
}

bool
gs_queue_push(gs_queue_t* queue, 
              void* element)
{
  if(queue->m_capacity == queue->m_num_elements)
  {
    return false;
  }

  queue->p_elements[queue->m_end] = element;
  queue->m_end++;
  if(queue->m_end == queue->m_capacity)
  {
    queue->m_end = 0;
  }
  queue->m_num_elements++;
  return true;
}


void*
gs_queue_pop(gs_queue_t* queue)
{
  if(queue->m_num_elements == 0)
  {
    return NULL;
  }

  void* ret = queue->p_elements[queue->m_start];

  queue->m_start++;
  if(queue->m_start == queue->m_capacity)
  {
    queue->m_start = 0;
  }
  queue->m_num_elements--;
  return ret;
}

void
gs_queue_clear(gs_queue_t* queue)
{
  queue->m_start = 0;
  queue->m_num_elements = 0;
  queue->m_end = 0;
}


uint32_t
gs_queue_count(gs_queue_t* queue)
{
  return queue->m_num_elements;
}
