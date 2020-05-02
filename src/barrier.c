
#include "platform.h"
#include "barrier.h"
#include "atomic_counter.h"
#include "tasking.h"


typedef struct gs_barrier_yield_data_t
{
  gs_barrier_t*  p_barrier;
  int32_t        m_num_processes;
} gs_barrier_yield_data_t;

static bool
do_resume(void* data)
{
  gs_barrier_yield_data_t* yield_data = (gs_barrier_yield_data_t*)data;
  int32_t res = (gs_atomic_counter_get(&yield_data->p_barrier->m_counter) - yield_data->m_num_processes);
  return res >= 0;
}

void
gs_barrier_wait(gs_barrier_t* barrier, int32_t nprocs)
{
  gs_atomic_counter_fetch_increment(&barrier->m_counter);
  uint32_t i = 0; 
  while(gs_atomic_counter_get(&barrier->m_counter) - nprocs < 0)
  {
    if( i > 1000)
    {
      gs_barrier_yield_data_t yield_data;
      yield_data.p_barrier = barrier;
      yield_data.m_num_processes = nprocs;
      gs_tasking_yield_func(do_resume, &yield_data);
      GS_ASSERT(false && "Mising implementation");
      i = 0;
    }
    ++i;
  }
}

void
gs_barrier_reset(gs_barrier_t* barrier)
{
  gs_atomic_counter_set(&barrier->m_counter, 0);
}
