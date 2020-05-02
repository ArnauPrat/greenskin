

#ifndef _GS_QUEUE_H_
#define _GS_QUEUE_H_

#include "platform.h"


typedef struct gs_queue_t
{
  uint32_t  m_start;
  uint32_t  m_end; 
  uint32_t  m_num_elements;
  uint32_t  m_capacity;
  void**    p_elements;
} gs_queue_t;

void
gs_queue_init(gs_queue_t* queue, 
              uint32_t initial_capacity);

void
gs_queue_release(gs_queue_t* queue);

bool
gs_queue_push(gs_queue_t* queue, 
              void* element);

void
gs_queue_clear(gs_queue_t* queue);

uint32_t
gs_queue_count(gs_queue_t* queue);


/**
 * \brief Pops an element from the queue
 *
 * @tparam T The type of the element to pop
 * \param queue The queue to pop the element from
 * \param element A pointer to store the popped element to. If the queue is
 * empty, nothing is copied to element.
 *
 * \return Returns True if an element was found, false otherwise.
 */
void*
gs_queue_pop(gs_queue_t* queue);

#endif 
