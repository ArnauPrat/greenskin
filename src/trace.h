

#ifndef _GS_TRACE_H_
#define _GS_TRACE_H_

#include "platform.h"
#include "task.h"

#define GS_TRACE_MAX_INFO_STRING_LENGTH 1024
#define GS_TRACE_MAX_NAME_STRING_LENGTH 128

#ifdef GS_ENABLE_TRACES
  #define GS_TRACE_RECORD(queue_id, event_type, category, name, info)\
                      gs_trace_record(queue_id, event_type, category, name, info);

  #define GS_TRACE_FLUSH()\
                      gs_trace_flush();

  #define GS_TRACE_ENABLE()\
                      gs_trace_record_enable();

  #define GS_TRACE_DISABLE()\
                      gs_trace_record_disable();
#elif
  #define GS_TRACE_RECORD(queue_id, event_type, name, info) 
  #define GS_TRACE_FLUSH() 
  #define GS_TRACE_ENABLE() 
  #define GS_TRACE_DISABLE() 
#endif


typedef enum gs_trace_event_type_t
{
  E_TRACE_UNKNOWN = 0,
  E_TRACE_TASK_START,
  E_TRACE_TASK_RESUME,
  E_TRACE_TASK_YIELD,
  E_TRACE_TASK_STOP,
  E_TRACE_NEW_FRAME,
} gs_trace_event_type_t;


typedef struct gs_trace_event_t
{
  uint64_t                  m_time_us;
  gs_trace_event_type_t     m_event_type;
  gs_task_category_t        m_task_category;
  char                      m_name[GS_TRACE_MAX_NAME_LEN];
  char                      m_info[GS_TRACE_MAX_INFO_LEN];
} gs_trace_event_t;


void
gs_trace_init(uint32_t num_queues);

void
gs_trace_release(void);

/**
 * \brief Gets the event queue for the specified queue
 *
 * \param queueid The queue id to get
 * \param count Pointer to variable to store the count in the array
 *
 * \return Returns the pointer to the queue
 */
gs_trace_event_t*
gs_trace_get_trace_event_array(uint32_t queueid, 
                               uint32_t* count);

/**
 * \brief Records a new frame timing event
 */
void
gs_trace_record(uint32_t queue_id, 
                gs_trace_event_type_t event_type,
                gs_task_category_t  tcategory, 
                const char* name, 
                const char* info);

/**
 * \brief Flushes all the queues
 */
void
gs_trace_flush(void);

/**
 * \brief Enables recording of traces
 */
void
gs_trace_record_enable(void);

/**
 * \brief Disables recording of traces
 */
void
gs_trace_record_disable(void);


#endif
