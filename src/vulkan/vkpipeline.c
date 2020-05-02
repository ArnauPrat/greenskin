
#include "vkpipeline.h"

VkPipelineShaderStageCreateInfo
gsvk_build_vertex_shader_stage(VkShaderModule* shader_module) 
{
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  info.module = *shader_module;
  info.pName = "main";
  info.flags = 0;
  info.pNext = NULL;
  info.pSpecializationInfo = NULL;
  return info;
}

VkPipelineShaderStageCreateInfo 
gsvk_build_fragment_shader_stage(VkShaderModule* shader_module) 
{
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  info.module = *shader_module;
  info.pName = "main";
  info.flags = 0;
  info.pNext = NULL;
  info.pSpecializationInfo = NULL;
  return info;
}
