
#include "aabb.h"
#include "vector.h"

bool
gs_aabb2_aabb2_overlap(gs_aabb2_t* a, gs_aabb2_t* b)
{
  if(a->m_max.m_x < b->m_min.m_x || b->m_max.m_x < a->m_min.m_x) return false;
  if(a->m_max.m_y < b->m_min.m_y || b->m_max.m_y < a->m_min.m_y) return false;
  return true;
}

bool
gs_aabb2_point2_overlap(gs_aabb2_t* a, gs_vec2float_t* b)
{

  return a->m_min.m_x <= b->m_x && 
         a->m_min.m_y <= b->m_y &&
         a->m_max.m_x >= b->m_x && 
         a->m_max.m_y >= b->m_y;
}
