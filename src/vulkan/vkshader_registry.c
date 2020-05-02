
#include "../platform.h"
#include "../log.h"
#include "../btree.h"
#include "../memory.h"
#include "../resources.h"
#include "../util.h"
#include "vkshader_registry.h"

static void
gsvk_load_shader_module(VkShaderModule* module, 
                   VkDevice device, 
                   const char* code, 
                   uint32_t code_length);

static void 
gsvk_unload_shader_module(VkShaderModule* module, 
                     VkDevice device);

typedef struct gsvk_shaderreg_entry_t
{
  uint32_t hash;
  VkShaderModule module;
} gsvk_shaderreg_entry_t;

typedef struct gsvk_shaderreg_t
{
  gs_btree_t      m_index;
  gs_pool_alloc_t m_entry_allocator;
  char*           m_filedata;
} gsvk_shaderreg_t;

static gsvk_shaderreg_t m_reg;

void
gsvk_shader_registry_init(void)
{
  m_reg = (gsvk_shaderreg_t){};
  gs_btree_init(&m_reg.m_index, &resources_allocator.m_super);
  gs_pool_alloc_init(&m_reg.m_entry_allocator, 
                     8, 
                     sizeof(gsvk_shaderreg_entry_t), 
                     GS_RESOURCES_SMALL_PAGE_SIZE,
                     &resources_allocator.m_super);

  m_reg.m_filedata = (char*)gs_alloc(&resources_allocator.m_super, 
                             GS_RESOURCES_LARGE_PAGE_SIZE, 
                             GS_RESOURCES_LARGE_PAGE_SIZE, 
                             GS_NO_HINT);
}

void
gsvk_shader_registry_release(void)
{
  gsvk_shader_registry_flush();
  gs_free(&resources_allocator.m_super, m_reg.m_filedata);
  gs_pool_alloc_release(&m_reg.m_entry_allocator);
  gs_btree_release(&m_reg.m_index);
}

void
gsvk_shader_registry_flush(void)
{
  gs_btree_iter_t iter;
  gs_btree_iter_init(&iter, &m_reg.m_index);
  while(gs_btree_iter_has_next(&iter))
  {
    gsvk_shaderreg_entry_t* entry = (gsvk_shaderreg_entry_t*)gs_btree_iter_next(&iter).p_value;
    gsvk_unload_shader_module(&entry->module, 
                              m_vkrenderer.m_logical_device);
  }
  gs_pool_alloc_flush(&m_reg.m_entry_allocator);
  gs_btree_clear(&m_reg.m_index);
}

VkShaderModule*
gsvk_shader_registry_load(const char* shader_name)
{
  uint32_t strhash = gs_hash(shader_name);
  gsvk_shaderreg_entry_t* entry = (gsvk_shaderreg_entry_t*) gs_btree_get(&m_reg.m_index, strhash);

  if(entry == NULL)
  {
    char buffer[GS_RESOURCES_MAX_FILE_PATH_LEN];
    if(!gs_resources_find_file(shader_name, 
                               (char*)buffer, 
                               GS_RESOURCES_MAX_FILE_PATH_LEN))
    {
      GS_LOG_ERROR(E_IO_FILE_NOT_FOUND, "Unable to locate shader resource %s", shader_name);
    }

    uint32_t length = gs_resources_read_file(buffer, 
                                             m_reg.m_filedata, 
                                             GS_RESOURCES_MAX_FILE_LEN);

    entry = (gsvk_shaderreg_entry_t*) gs_pool_alloc_alloc(&m_reg.m_entry_allocator, 
                                                          8, 
                                                          sizeof(gsvk_shaderreg_entry_t), 
                                                          GS_NO_HINT);
    entry->hash = strhash;
    gsvk_load_shader_module(&entry->module, 
                            m_vkrenderer.m_logical_device, 
                            m_reg.m_filedata, 
                            length);
    gs_btree_insert(&m_reg.m_index, strhash, entry);
  }

  return &entry->module;
}

void
gsvk_load_shader_module(VkShaderModule* module, 
                        VkDevice device, 
                        const char* code, 
                        uint32_t code_length) 
{
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code_length;
  create_info.pCode = (const uint32_t*)(code);

  if (vkCreateShaderModule(device, 
                           &create_info, 
                           NULL, 
                           module) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                  "failed to create shader module!");
  }
}

void 
gsvk_unload_shader_module(VkShaderModule* module, 
                          VkDevice device) 
{
  vkDestroyShaderModule(device, *module, NULL);
}

/*
VkPipelineShaderStageCreateInfo
build_vertex_shader_stage(VkShaderModule shader_module) 
{
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  info.module = shader_module;
  info.pName = "main";
  info.flags = 0;
  info.pNext = NULL;
  info.pSpecializationInfo = NULL;
  return info;
}

VkPipelineShaderStageCreateInfo 
build_fragment_shader_stage(VkShaderModule shader_module) 
{
  VkPipelineShaderStageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  info.module = shader_module;
  info.pName = "main";
  info.flags = 0;
  info.pNext = NULL;
  info.pSpecializationInfo = NULL;
  return info;
}
*/
