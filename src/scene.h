
#ifndef _GS_SCENE_H_
#define _GS_SCENE_H_


#include "matrix.h"


typedef struct gs_scene_t
{
  gs_mat4x4float_t m_proj_matrix;
  gs_mat4x4float_t m_view_matrix;
} gs_scene_t;


void
gs_scene_init(gs_scene_t* scene);

void
gs_scene_release(gs_scene_t* scene);


#endif
