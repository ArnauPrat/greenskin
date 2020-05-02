

#ifndef _GS_VECTOR_H_
#define _GS_VECTOR_H_

#include "platform.h"

// 2D VECTORS
typedef struct gs_vec2float_t
{
  union 
  {
    float m_data[2];
    struct { float m_x; float m_y; };
    struct { float m_s; float m_t; };
  };
} gs_vec2float_t;

typedef struct gs_vec2double_t
{
  union 
  {
    double m_data[2];
    struct { double m_x; double m_y; };
    struct { double m_s; double m_t; };
  };
} gs_vec2double_t;

typedef struct gs_vec2i32_t
{
  union 
  {
    int32_t m_data[2];
    struct { int32_t m_x; int32_t m_y; };
    struct { int32_t m_s; int32_t m_t; };
  };
} gs_vector2i32_t;

typedef struct gs_vec2u32_t
{
  union 
  {
    int32_t m_data[2];
    struct { uint32_t m_x; uint32_t m_y; };
    struct { uint32_t m_s; uint32_t m_t; };
  };
} gs_vec2u32_t;


// 3D VECTORS
typedef struct gs_vec3float_t
{
  union 
  {
    float m_data[3];
    struct { float m_x; float m_y; float m_z;};
    struct { float m_s; float m_t; float m_p;};
    struct { float m_r; float m_g; float m_b;};
  };
} gs_vec3float_t;

typedef struct gs_vec3double_t
{
  union 
  {
    double m_data[3];
    struct { double m_x; double m_y; double m_z;};
    struct { double m_s; double m_t; double m_p;};
    struct { double m_r; double m_g; double m_b;};
  };
} gs_vec3double_t;

typedef struct gs_vec3i32_t
{
  union 
  {
    int32_t m_data[3];
    struct { int32_t m_x; int32_t m_y; int32_t m_z;};
    struct { int32_t m_s; int32_t m_t; int32_t m_p;};
    struct { int32_t m_r; int32_t m_g; int32_t m_b;};
  };
} gs_vec3i32_t;

typedef struct gs_vec3u32_t
{
  union 
  {
    int32_t m_data[3];
    struct { uint32_t m_x; uint32_t m_y; uint32_t m_z;};
    struct { uint32_t m_s; uint32_t m_t; uint32_t m_p;};
    struct { uint32_t m_r; uint32_t m_g; uint32_t m_b;};
  };
} gs_vec3u32_t;

// 4D VECTORS 

typedef struct gs_vec4float_t
{
  union 
  {
    float m_data[4];
    struct { float m_x; float m_y; float m_z; float m_w;};
    struct { float m_s; float m_t; float m_p; float m_q;};
    struct { float m_r; float m_g; float m_b; float m_a;};
  };
} gs_vec4float_t;

typedef struct gs_vec4double_t
{
  union 
  {
    double m_data[4];
    struct { double m_x; double m_y; double m_z; double m_w;};
    struct { double m_s; double m_t; double m_p; double m_q;};
    struct { double m_r; double m_g; double m_b; double m_a;};
  };
} gs_vec4double_t;

typedef struct gs_vec4i32_t
{
  union 
  {
    int32_t m_data[4];
    struct {int32_t m_x; int32_t m_y; int32_t m_z; int32_t m_w;};
    struct {int32_t m_s; int32_t m_t; int32_t m_p; int32_t m_q;};
    struct {int32_t m_r; int32_t m_g; int32_t m_b; int32_t m_a;};
  };
} gs_vec4i32_t;

typedef struct gs_vec4u32_t
{
  union 
  {
    int32_t m_data[4];
    struct { uint32_t m_x; uint32_t m_y; uint32_t m_z; uint32_t m_w;};
    struct { uint32_t m_s; uint32_t m_t; uint32_t m_p; uint32_t m_q;};
    struct { uint32_t m_r; uint32_t m_g; uint32_t m_b; uint32_t m_a;};
  };
} gs_vec4u32_t;

#endif 
