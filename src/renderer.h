

#ifndef _GS_RENDERER_H_
#define _GS_RENDERER_H_ 

#include "config.h"
#include "vector.h"
#include "matrix.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct gs_device_properties_t 
{
  size_t m_uniform_offset_alignment;
} gs_device_properties_t;

enum gs_render_object_type_t 
{
  E_MESH,
};

typedef struct gs_material_desc_t 
{
  gs_vec4float_t   m_color;
} gs_material_desc_t;

typedef struct gs_render_mesh_uniform_t 
{
  gs_mat4x4float_t       m_model_matrix;
  gs_material_desc_t     m_material;
} gs_render_mesh_uniform_t;

typedef struct gs_rendering_scene_t
{
} gs_rendering_scene_t;


/**
 * @brief Initializes the rendering subsystem
 *
 * @param config
 */
void
gs_renderer_init(const gs_config_t* config, 
                 GLFWwindow* window);

/**
 * @brief Terminates the rendering subsystem
 */
void
gs_renderer_release(void);

/**
 * @brief Frees resources and starts the execution of a frame
 */
void
gs_renderer_begin_frame(void);

/**
 * @brief Draws a frame
 */
void
gs_renderer_end_frame(void);

/**
 * \brief Initializes the gui subsystem
 */
void
gs_gui_init(void);

/**
 * \brief Terminates the gui subsystem
 */
void
gs_gui_release(void);

/**
 * \brief Gets the device properties of the particular renderer implementation
 *
 * \return Returns a TnaDeviceProperties structure with the property values
 */
gs_device_properties_t
gs_get_device_properties(void);

#endif /* ifndef _TNA_RENDERING_H_ */
