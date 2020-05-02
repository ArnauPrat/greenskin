


#ifndef _GS_MATRIX_H_
#define _GS_MATRIX_H_ value

#include "platform.h"
#include "vector.h"

// MATRIX4x4

typedef struct gs_mat4x4float_t
{
  gs_vec4float_t m_col[4];
} gs_mat4x4float_t;

typedef struct gs_mat4x4double_t
{
  gs_vec4double_t m_col[4];
} gs_mat4x4double_t;


typedef struct gs_mat4x4i32_t
{
  gs_vec4i32_t m_col[4];
} gs_mat4x4i32_t;

typedef struct gs_mat4x4u32_t
{
  gs_vec4u32_t m_col[4];
} gs_mat4x4u32_t;


#endif 
