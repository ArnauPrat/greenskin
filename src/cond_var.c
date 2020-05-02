

#include "cond_var.h"
#include "mutex.h"

void
gs_cond_var_init(gs_cond_var_t* cond_var)
{
#ifdef GS_OS_LINUX
  pthread_cond_init(&cond_var->m_pthread_cond, 
                    NULL);
#endif
}

void
gs_cond_var_release(gs_cond_var_t* cond_var)
{
#ifdef GS_OS_LINUX
  pthread_cond_destroy(&cond_var->m_pthread_cond);
#endif
}

void
gs_cond_var_wait(gs_cond_var_t* cond_var, 
                 gs_mutex_t* mutex)
{
#ifdef GS_OS_LINUX
  pthread_cond_wait(&cond_var->m_pthread_cond, 
                    &mutex->m_mutex);
#endif
}

void
gs_cond_var_notify_one(gs_cond_var_t* cond_var)
{
#ifdef GS_OS_LINUX
  pthread_cond_signal(&cond_var->m_pthread_cond);
#endif
}

void
gs_cond_var_notify_all(gs_cond_var_t*  cond_var)
{
#ifdef GS_OS_LINUX
  pthread_cond_broadcast(&cond_var->m_pthread_cond);
#endif
}
