
#include "thread.h"
#include "log.h"

#ifdef GS_OS_LINUX
#include <sched.h>
void* pthread_handler(void* arg)
{
  gs_task_t* task = (gs_task_t*) arg;
  task->p_fp(task->p_args);
  return NULL;
}
#endif



/**
 * \brief Starts a thread with the given task
 *
 * \param thread The thread to start
 * \param task  The task to run with the thread
 */
void
gs_thread_start(gs_thread_t* thread, 
                gs_task_t* task)
{
#ifdef GS_OS_LINUX
  pthread_create(&thread->m_pthread, 
                 NULL, 
                 pthread_handler, 
                 task);
#endif
}

/**
 * \brief Joins a thread
 *
 * \param thread The thread to join
 */
void
gs_thread_join(gs_thread_t* thread)
{
#ifdef GS_OS_LINUX
  void* ptr;
  pthread_join(thread->m_pthread, &ptr);
#endif
}

/**
 * \brief Kills a thread
 *
 * \param thread The thread to kill
 */
void
gs_thread_kill(gs_thread_t* thread)
{
#ifdef GS_OS_LINUX
  pthread_cancel(thread->m_pthread);
#endif
}

bool
gs_thread_set_affinity(gs_thread_t* thread, 
                       uint32_t cpuid)
{
#ifdef GS_OS_LINUX
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpuid, &cpuset);
  uint32_t res = pthread_setaffinity_np(thread->m_pthread,
                                        sizeof(cpu_set_t), 
                                        &cpuset);
  return res == 0;
#endif
}

bool
gs_thread_set_main_affinity(uint32_t cpuid)
{
#ifdef GS_OS_LINUX
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpuid, &cpuset);
  uint32_t res = sched_setaffinity(0, sizeof(cpuset), &cpuset);
  return res == 0;
#endif
}
