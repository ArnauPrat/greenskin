

#ifndef _GS_COND_VAR_H_
#define _GS_COND_VAR_H_

#include "platform.h"
#include "mutex.h"

#ifdef GS_OS_LINUX
#include <pthread.h>
#endif

typedef struct gs_cond_var_t
{
#ifdef GS_OS_LINUX
  pthread_cond_t m_pthread_cond;
#endif
} gs_cond_var_t;

void
gs_cond_var_init(gs_cond_var_t* cond_var);

void
gs_cond_var_release(gs_cond_var_t* cond_var);

void
gs_cond_var_wait(gs_cond_var_t* cond_var, 
                 gs_mutex_t* mutex);

void
gs_cond_var_notify_one(gs_cond_var_t* cond_var);

void
gs_cond_var_notify_all(gs_cond_var_t*  cond_var);
                   

#endif
