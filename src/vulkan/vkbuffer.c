

#include "../log.h"
#include "vkrenderer.h"
#include "vkbuffer.h"
#include <string.h>

void 
gsvk_buffer_init(gsvk_buffer_t* buffer, 
                 VkDeviceSize size, 
                 VkBufferUsageFlags buffer_usage, 
                 VkMemoryPropertyFlagBits mem_props)
{
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = buffer_usage;
  bufferInfo.sharingMode =  VK_SHARING_MODE_EXCLUSIVE;

  if(vkCreateBuffer(m_vkrenderer.m_logical_device, 
                    &bufferInfo, 
                    NULL, 
                    &buffer->m_bhandle) != VK_SUCCESS)  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "Vulkan: failed to allocate memory buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_vkrenderer.m_logical_device, 
                                buffer->m_bhandle,
                                &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = gsvk_find_memory_type(&memRequirements, mem_props);

  if (vkAllocateMemory(m_vkrenderer.m_logical_device, 
                       &allocInfo, 
                       NULL,
                       &buffer->m_mhandle) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(m_vkrenderer.m_logical_device,
                    buffer->m_bhandle,
                    buffer->m_mhandle,
                    0);
}

void 
gsvk_buffer_release(gsvk_buffer_t* buffer) 
{
  
  vkFreeMemory(m_vkrenderer.m_logical_device, 
               buffer->m_mhandle,
               NULL);

  vkDestroyBuffer(m_vkrenderer.m_logical_device, 
                 buffer->m_bhandle,
                 NULL);
  return;
}


void 
gsvk_vbuffer_init(gsvk_vbuffer_t* buffer, 
                  VkDevice device,
                  VkQueue queue,
                  VkCommandPool pool,
                  void* vertices, 
                  size_t size) 
{

  gsvk_buffer_t staging_buffer;
  gsvk_buffer_init(&staging_buffer, 
                   size, 
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


  void* tmp_data = NULL;
  vkMapMemory(m_vkrenderer.m_logical_device,
              staging_buffer.m_mhandle, 
              0, 
              size, 
              0, 
              &tmp_data);
  memcpy(tmp_data, vertices, (size_t) size);
  vkUnmapMemory(m_vkrenderer.m_logical_device,
                staging_buffer.m_mhandle);

  gsvk_buffer_init(&buffer->m_buffer, 
                   size, 
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  gsvk_copy_buffer(&buffer->m_buffer, 
                   &staging_buffer, 
                   device,
                   queue,
                   pool, 
                   size);

  gsvk_buffer_release(&staging_buffer);

  return;
}

void
gsvk_vbuffer_release(gsvk_vbuffer_t* buffer)
{
  gsvk_buffer_release(&buffer->m_buffer);
}

void 
gsvk_ibuffer_init(gsvk_ibuffer_t* buffer, 
                  VkDevice device,
                  VkQueue queue,
                  VkCommandPool pool,
                  void* indices, 
                  size_t size) 
{

  gsvk_buffer_t staging_buffer;
  gsvk_buffer_init(&staging_buffer, 
                   size, 
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


  void* tmp_data = NULL;
  vkMapMemory(m_vkrenderer.m_logical_device,
              staging_buffer.m_mhandle, 
              0, 
              size, 
              0, 
              &tmp_data);
  memcpy(tmp_data, indices, (size_t) size);
  vkUnmapMemory(m_vkrenderer.m_logical_device,
                staging_buffer.m_mhandle);

  gsvk_buffer_init(&buffer->m_buffer, 
                   size, 
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  gsvk_copy_buffer(&buffer->m_buffer, 
                   &staging_buffer, 
                   device,
                   queue,
                   pool, 
                   size);

  gsvk_buffer_release(&staging_buffer);
  return;
}

void
gsvk_ibuffer_release(gsvk_ibuffer_t* buffer)
{
  gsvk_buffer_release(&buffer->m_buffer);
}

void
gsvk_copy_buffer(gsvk_buffer_t* dst,
                 gsvk_buffer_t* src,
                 VkDevice device,
                 VkQueue queue,
                 VkCommandPool command_pool,
                 VkDeviceSize size) 
{
  VkCommandBuffer command_buffer = gsvk_begin_single_time_commands(device, 
                                                                   command_pool);

  VkBufferCopy copyRegion = {};
  copyRegion.srcOffset = 0; // Optional
  copyRegion.dstOffset = 0; // Optional
  copyRegion.size = size;
  vkCmdCopyBuffer(command_buffer, 
                  src->m_bhandle, 
                  dst->m_bhandle, 
                  1, 
                  &copyRegion);

  gsvk_end_single_time_commands(device, 
                                queue, 
                                command_pool, 
                                command_buffer);

  return;
}
