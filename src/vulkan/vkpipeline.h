

#ifndef _GS_VKPIPELINE_H_
#define _GS_VKPIPELINE_H_

#include "vkrenderer.h"

VkPipelineShaderStageCreateInfo
gsvk_build_vertex_shader_stage(VkShaderModule* shader_module);

VkPipelineShaderStageCreateInfo 
gsvk_build_fragment_shader_stage(VkShaderModule* shader_module);

#endif
