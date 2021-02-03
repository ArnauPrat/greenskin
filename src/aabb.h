


#ifndef _GS_AABB_H_
#define _GS_AABB_H_

#include "vector.h"

typedef struct gs_aabb3_t 
{
  gs_vec3float_t m_min;
  gs_vec3float_t m_max;
} gs_aabb3_t;

typedef struct gs_aabb2_t 
{
  gs_vec2float_t m_min;
  gs_vec2float_t m_max;
} gs_aabb2_t;

#endif /* ifndef _GS_AABB_H_ */
