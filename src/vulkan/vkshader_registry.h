

#ifndef _GS_VKSHADER_REGISTRY_H_
#define _GS_VKSHADER_REGISTRY_H_ 

#include "vkrenderer.h"

/**
 * \brief Initializes the shader registry
 */
void
gsvk_shader_registry_init(void);

/**
 * \brief Releases the shader registry
 */
void
gsvk_shader_registry_release(void);

/**
 * \brief Flushes the shader registry
 */
void
gsvk_shader_registry_flush(void);

/**
 * \brief Loads a shader module if it was not previously loaded
 *
 * \param shader_name The name of the shader module to load
 *
 * \return Returns a pointer to the loaded shader module
 */
VkShaderModule*
gsvk_shader_registry_load(const char* shader_name); 

#endif 
