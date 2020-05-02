

#ifndef _GS_ATOMIC_COUNTER_H_
#define _GS_ATOMIC_COUNTER_H_

#include "platform.h"

typedef struct gs_atomic_counter_t 
{
  volatile int32_t m_counter;
} gs_atomic_counter_t;


/**
 * @brief Sets the value of the counter
 *
 * @param value
 */
int32_t 
gs_atomic_counter_set(gs_atomic_counter_t* counter, 
                   int32_t value);

/**
 * @brief Fetches and Increments the value of the counter
 *
 * @return 
 */
int32_t 
gs_atomic_counter_fetch_increment(gs_atomic_counter_t* counter);

/**
 * @brief Fetches and Decrements the value of the counter
 */
int32_t 
gs_atomic_counter_fetch_decrement(gs_atomic_counter_t* counter);

/**
 * \brief Gets the current value of the atomic counter 
 *
 * \param counter The atomic counter to get the value from
 *
 * \return Returns the current value of the atomic counter
 */
int32_t
gs_atomic_counter_get(gs_atomic_counter_t* counter);

/**
 * @brief Blocks until the counter is set to zero
 */
void 
gs_atomic_counter_join(gs_atomic_counter_t* counter);

#endif /* ifndef _SMILE_TASKING_SYNC_COUNTER_H_ */
