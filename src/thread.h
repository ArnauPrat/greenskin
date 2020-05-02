

#ifndef _GS_THREAD_H_
#define _GS_THREAD_H_ value

#include "platform.h"
#include "task.h"

#ifdef GS_OS_LINUX
#include <pthread.h>
#endif


typedef struct gs_thread_t
{
#ifdef GS_OS_LINUX
  pthread_t m_pthread;
#endif
} gs_thread_t;


/**
 * \brief Starts a thread with the given task
 *
 * \param thread The thread to start
 * \param task  The task to run with the thread
 */
void
gs_thread_start(gs_thread_t* thread, 
                gs_task_t* task);

/**
 * \brief Joins a thread
 *
 * \param thread The thread to join
 */
void
gs_thread_join(gs_thread_t* thread);

/**
 * \brief Kills a thread
 *
 * \param thread The thread to kill
 */
void
gs_thread_kill(gs_thread_t* thread);

/**
 * \brief Sets the affinity for the given thread
 *
 * \param thread The thread to set the affinity for
 * \param cpuid The id of the cpu to set the affinity to
 */
bool
gs_thread_set_affinity(gs_thread_t* thread, 
                       uint32_t cpuid);

/**
 * \brief Sets the affinity of the main thread
 *
 * \param cpuid The id of the cpu to set the affinity to
 */
bool
gs_thread_set_main_affinity(uint32_t cpuid);



#endif /* ifndef _GS_THREAD_H_ */
