
#include "trace.h"
#include "mutex.h" 
#include "memory.h"

#include <string.h>
#include <time.h>


static uint32_t m_num_queues = 0;

/**
 * \brief Trace event arrays for each of the threads
 */
static gs_trace_event_t**  p_trace_event_arrays = NULL;

/**
 * \brief The trace event arrays count
 */
static uint32_t*        m_trace_event_count = NULL;

/**
 * \brief The trace event arrays capacity
 */
static uint32_t*        m_trace_event_capacity = NULL;

/**
 * \brief Mutex for event arrays
 */
static gs_mutex_t       m_trace_event_mutex;

static bool             m_trace_record_enabled = false;

void
gs_trace_init(uint32_t num_queues)
{
  gs_mem_allocator_t* allocator = gs_get_global_allocator();
  m_num_queues = num_queues;
  p_trace_event_arrays = (gs_trace_event_t**)gs_alloc(allocator, 
                                                      64, 
                                                      sizeof(gs_trace_event_t*)*m_num_queues, 
                                                      GS_NO_HINT);

  m_trace_event_count  = (uint32_t*)gs_alloc(allocator, 
                                             64, 
                                             sizeof(uint32_t)*m_num_queues, 
                                             GS_NO_HINT);

  m_trace_event_capacity = (uint32_t*)gs_alloc(allocator, 
                                               64, 
                                               sizeof(uint32_t)*m_num_queues, 
                                               GS_NO_HINT);

  for(uint32_t i = 0; i < m_num_queues; ++i) 
  {
    m_trace_event_count[i] = 0;
    m_trace_event_capacity[i] = 1024;
    p_trace_event_arrays[i] = (gs_trace_event_t*)gs_alloc(allocator, 
                                       64, 
                                       sizeof(gs_trace_event_t)*1024,
                                       GS_NO_HINT);
  }

  gs_mutex_init(&m_trace_event_mutex);
}

void
gs_trace_release()
{
  gs_mem_allocator_t* allocator = gs_get_global_allocator();
  for (uint32_t i = 0; i < m_num_queues; ++i) 
  {
    gs_free(allocator, p_trace_event_arrays[i]);
  }

  gs_free(allocator, p_trace_event_arrays);
  gs_free(allocator, m_trace_event_capacity);
  gs_free(allocator, m_trace_event_count);
  gs_mutex_release(&m_trace_event_mutex);
}

void
gs_trace_record(uint32_t queue_id, 
             gs_trace_event_type_t event_type,
             const char* name, 
             const char* info)
{
  gs_mem_allocator_t* allocator = gs_get_global_allocator();
  gs_mutex_lock(&m_trace_event_mutex);
  if(m_trace_record_enabled)
  {
    gs_trace_event_t trace_event = {0};
    trace_event.m_time_us = clock() / (CLOCKS_PER_SEC/(1000*1000));
    trace_event.m_event_type = event_type;
    if(info != NULL)
    {
      strncpy(trace_event.m_info, info, GS_TRACE_MAX_INFO_LEN-1);
      trace_event.m_info[GS_TRACE_MAX_INFO_LEN-1] = '\0';
    }
    else
    {
      trace_event.m_info[0] = '\0';
    }

    if(info != NULL)
    {
      strncpy(trace_event.m_name, name, GS_TRACE_MAX_NAME_LEN-1);
      trace_event.m_name[GS_TRACE_MAX_NAME_LEN-1] = '\0';
    }
    else
    {
      trace_event.m_name[0] = '\0';
    }

    if(m_trace_event_count[queue_id] >= m_trace_event_capacity[queue_id])
    {
      uint32_t new_capacity = m_trace_event_capacity[queue_id]*2;
      gs_trace_event_t* new_buffer = gs_alloc(allocator, 
                                              64, 
                                              sizeof(gs_trace_event_t)*new_capacity, 
                                              GS_NO_HINT);
      memcpy(new_buffer, p_trace_event_arrays[queue_id], sizeof(gs_trace_event_t)*m_trace_event_count[queue_id]);
      m_trace_event_capacity[queue_id] = new_capacity;
      gs_free(allocator, p_trace_event_arrays[queue_id]);
      p_trace_event_arrays[queue_id] = new_buffer;
    }

    (p_trace_event_arrays[queue_id])[m_trace_event_count[queue_id]] = trace_event;
    m_trace_event_count[queue_id]++;
  }
  gs_mutex_unlock(&m_trace_event_mutex);
}

gs_trace_event_t*
trace_get_trace_event_array(uint32_t queue_id, uint32_t* count)
{
  GS_PERMA_ASSERT(!m_trace_record_enabled && "Trace arrays cannot be accessed while recording traces");

  *count = m_trace_event_count[queue_id];
  return p_trace_event_arrays[queue_id];
}

void
gs_trace_flush()
{
  gs_mutex_lock(&m_trace_event_mutex);
  for(uint32_t i = 0; i < m_num_queues; ++i)
  {
    m_trace_event_count[i] = 0;
  }
  gs_mutex_unlock(&m_trace_event_mutex);
}

void
gs_trace_record_enable()
{
  gs_mutex_lock(&m_trace_event_mutex);
  m_trace_record_enabled = true;
  gs_mutex_unlock(&m_trace_event_mutex);
}

void
gs_trace_record_disable()
{
  gs_mutex_lock(&m_trace_event_mutex);
  m_trace_record_enabled = false;
  gs_mutex_unlock(&m_trace_event_mutex);
}
