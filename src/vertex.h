

#ifndef _GS_VERTEX_H_
#define _GS_VERTEX_H_

#include "vector.h"

typedef struct gs_vertex_t 
{
  gs_vec3float_t m_position;
  gs_vec3float_t m_color;
  gs_vec2float_t m_tex_coord;
} gs_vertex_t;
  
#endif

