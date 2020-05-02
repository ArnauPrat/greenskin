
#ifndef _GS_BARRIER_H
#define _GS_BARRIER_H value

#include "atomic_counter.h"

typedef struct gs_barrier_t 
{
  gs_atomic_counter_t  m_counter;
} gs_barrier_t;

void
gs_barrier_wait(gs_barrier_t* barrier, int32_t nprocs);

void
gs_barrier_reset(gs_barrier_t* barrier);
  
#endif /* ifndef _GS_BARRIER_H */
