
#include "vkimage.h"
#include "vkrenderer.h"
#include "../log.h"

void
gsvk_image_init(gsvk_image_t* image, 
                uint32_t width, 
                uint32_t height, 
                VkFormat format, 
                VkImageTiling tiling, 
                VkImageUsageFlags usage, 
                gsvk_mem_type_t mem_type
                )
{

  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(m_vkrenderer.m_logical_device, 
                    &image_info, 
                    NULL, 
                    &image->m_ihandle) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to create image!");
  }

  gsvk_mem_alloc_image(&m_vkrenderer.m_vk_mem_alloc, 
                       &image->m_ihandle, 
                       mem_type, 
                       &image->m_mhandle);

  /*VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(m_vkrenderer.m_logical_device, 
                               image->m_ihandle, 
                               &mem_reqs);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = mem_reqs.size;
  allocInfo.memoryTypeIndex = gsvk_find_memory_type(&mem_reqs, mem_props);

  if (vkAllocateMemory(m_vkrenderer.m_logical_device,
                       &allocInfo, 
                       NULL, 
                       &image->m_mhandle) != VK_SUCCESS)
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to allocate image memory!");
  }
  */


  vkBindImageMemory(m_vkrenderer.m_logical_device,
                    image->m_ihandle, 
                    image->m_mhandle.m_mhandle, 
                    image->m_mhandle.m_offset);

  image->m_width = width;
  image->m_height = height;

  return;
}



/**
 * \brief Releases the vulkan image
 *
 * \param image The image to release
 */
void
gsvk_image_release(gsvk_image_t* image)
{
  /*vkFreeMemory(m_vkrenderer.m_logical_device, 
               image->m_mhandle,
               NULL);
               */
  gsvk_mem_alloc_free(&m_vkrenderer.m_vk_mem_alloc, 
                      &image->m_mhandle);

  vkDestroyImage(m_vkrenderer.m_logical_device, 
                 image->m_ihandle,
                 NULL);
}

void 
gsvk_image_transition_layout(gsvk_image_t* image, 
                             VkDevice device,
                             VkCommandPool command_pool,
                             VkQueue queue,
                             VkFormat format, 
                             VkImageLayout oldLayout, 
                             VkImageLayout newLayout) 
{
  VkCommandBuffer command_buffer = gsvk_begin_single_time_commands(device, command_pool);

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;

  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  barrier.image = image->m_ihandle;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) // has stencil buffer
    {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }


  VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
      && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } 
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 
           && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
  {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } 
  else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED 
           && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else 
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "unsupported layout transition!");
  }


  vkCmdPipelineBarrier(command_buffer,
                       sourceStage, 
                       destinationStage,
                       0,
                       0, 
                       NULL,
                       0, 
                       NULL,
                       1, 
                       &barrier);

  gsvk_end_single_time_commands(device, 
                                queue, 
                                command_pool, 
                                command_buffer);
}
