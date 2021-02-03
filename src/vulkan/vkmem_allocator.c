


#include "vkmem_allocator.h"
#include "vkrenderer.h"
#include "log.h"
#include <string.h>

void
gsvk_mem_alloc_init(gsvk_mem_alloc_t* mem_alloc)
{
  memset(mem_alloc, 0, sizeof(gsvk_mem_alloc_t));
  gs_mutex_init(&mem_alloc->m_mutex);

  // Getting the maximum number of possible memory allocations
  //mem_alloc->m_max_num_allocs = m_vkrenderer.m_pd_properties.limits.maxMemoryAllocationCount;

  // Looking for buffer device memory
  VkBufferCreateInfo buffer_info = {};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = GS_RENDERING_VMEM_BFF_DEVICE;
  buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_info.sharingMode =  VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer bhandle_device;
  if(vkCreateBuffer(m_vkrenderer.m_logical_device, 
                    &buffer_info, 
                    NULL, 
                    &bhandle_device) != VK_SUCCESS)  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to create memory buffer at vulkan allocator!");
  }

  VkMemoryRequirements bmem_reqs_device;
  vkGetBufferMemoryRequirements(m_vkrenderer.m_logical_device, 
                                bhandle_device,
                                &bmem_reqs_device);

  VkMemoryAllocateInfo balloc_device_info = {};
  balloc_device_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  balloc_device_info.allocationSize = bmem_reqs_device.size;
  balloc_device_info.memoryTypeIndex = gsvk_find_memory_type(&bmem_reqs_device,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  vkDestroyBuffer(m_vkrenderer.m_logical_device, 
                  bhandle_device, 
                  NULL);

  // Looking for buffer host memory
  VkBuffer bhandle_host;
  if(vkCreateBuffer(m_vkrenderer.m_logical_device, 
                    &buffer_info, 
                    NULL, 
                    &bhandle_host) != VK_SUCCESS)  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to create memory buffer at vulkan allocator!");
  }

  VkMemoryRequirements bmem_reqs_host;
  vkGetBufferMemoryRequirements(m_vkrenderer.m_logical_device, 
                                bhandle_host,
                                &bmem_reqs_host);

  vkDestroyBuffer(m_vkrenderer.m_logical_device, 
                  bhandle_host, 
                  NULL);

  VkMemoryAllocateInfo balloc_host_info = {};
  balloc_host_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  balloc_host_info.allocationSize = bmem_reqs_host.size;
  balloc_host_info.memoryTypeIndex = gsvk_find_memory_type(&bmem_reqs_host,  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  uint32_t bmem_device_idx = balloc_device_info.memoryTypeIndex;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_BFF_DEVICE] = bmem_device_idx;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_BFF_DEVICE] = m_vkrenderer.m_pd_mem_properties.memoryTypes[bmem_device_idx].heapIndex;

  uint32_t bmem_host_idx = balloc_host_info.memoryTypeIndex;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_BFF_HOST] = bmem_host_idx;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_BFF_HOST] = m_vkrenderer.m_pd_mem_properties.memoryTypes[bmem_host_idx].heapIndex;


  VkFormat candidates[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  VkFormat format = gsvk_find_supported_format(m_vkrenderer.m_physical_device,
                                               candidates, 
                                               sizeof(candidates),
                                               VK_IMAGE_TILING_OPTIMAL,
                                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

  // Looking for image device memory
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = 1024;
  image_info.extent.height = 1024;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkImage ihandle;
  if (vkCreateImage(m_vkrenderer.m_logical_device, 
                    &image_info, 
                    NULL, 
                    &ihandle) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "failed to create image at vulkan memory allocator!");
  }

  VkMemoryRequirements imem_reqs;
  vkGetImageMemoryRequirements(m_vkrenderer.m_logical_device, 
                               ihandle, 
                               &imem_reqs);

  VkMemoryAllocateInfo img_alloc_info = {};
  img_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  img_alloc_info.allocationSize = GS_RENDERING_VMEM_IMG_DEVICE;
  img_alloc_info.memoryTypeIndex = gsvk_find_memory_type(&imem_reqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkDestroyImage(m_vkrenderer.m_logical_device, 
                 ihandle,
                 NULL);

  uint32_t imem_idx = img_alloc_info.memoryTypeIndex;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_IMG_DEVICE] = imem_idx;
  mem_alloc->m_vk_mem_types[E_GSVK_MEM_TYPE_IMG_DEVICE] = m_vkrenderer.m_pd_mem_properties.memoryTypes[imem_idx].heapIndex;


  // Checking sizes
  if(mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_BFF_DEVICE]  == 
     mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_IMG_DEVICE] )
  {
    uint32_t heap_idx = mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_BFF_DEVICE];
    if(m_vkrenderer.m_pd_mem_properties.memoryHeaps[heap_idx].size < 
       (GS_RENDERING_VMEM_IMG_DEVICE + GS_RENDERING_VMEM_BFF_DEVICE))
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Insufficient device VMEM size");
    }
  }
  else
  {
    uint32_t heap_idx = mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_BFF_DEVICE];
    if(m_vkrenderer.m_pd_mem_properties.memoryHeaps[heap_idx].size < 
       (GS_RENDERING_VMEM_BFF_DEVICE))
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Insufficient BUFFER DEVICE VMEM size");
    }

    heap_idx = mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_IMG_DEVICE];
    if(m_vkrenderer.m_pd_mem_properties.memoryHeaps[heap_idx].size < 
       (GS_RENDERING_VMEM_IMG_DEVICE))
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Insufficient IMAGE DEVICE VMEM size");
    }
  }

  uint32_t heap_idx = mem_alloc->m_vk_mem_heaps[E_GSVK_MEM_TYPE_BFF_HOST];
  if(m_vkrenderer.m_pd_mem_properties.memoryHeaps[heap_idx].size < 
     (GS_RENDERING_VMEM_BFF_HOST))
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Insufficient BUFFER HOST VMEM size");
  }

  // Querying for the maximum allocatable memory chunk
  /*VkPhysicalDeviceMaintenance3Properties pd_man_props3;
  pd_man_props3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES; 
  VkPhysicalDeviceProperties2 pd_props2;
  pd_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
  pd_props2.pNext = &pd_man_props3;

  vkGetPhysicalDeviceProperties2(m_vkrenderer.m_physical_device, 
                                 &pd_props2);


  uint32_t chunk_size = pd_man_props3.maxMemoryAllocationSize;
  if(chunk_size < GS_RENDERING_VMEM_PAGE_SIZE)
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "DEVICE VMEM page size is too large");
  }
  */

  // Allocating buffer device memory
  if(vkAllocateMemory(m_vkrenderer.m_logical_device, 
                      &balloc_device_info, 
                      NULL, 
                      &mem_alloc->m_vk_mem_handles[E_GSVK_MEM_TYPE_BFF_DEVICE]) != VK_SUCCESS)
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Unable to allocate BUFFER DEVICE VMEM");
  }
  mem_alloc->m_vk_mem_sizes[E_GSVK_MEM_TYPE_BFF_DEVICE] = balloc_device_info.allocationSize;

  // Allocating buffer host memory
  if(vkAllocateMemory(m_vkrenderer.m_logical_device, 
                      &balloc_host_info, 
                      NULL, 
                      &mem_alloc->m_vk_mem_handles[E_GSVK_MEM_TYPE_BFF_HOST]) != VK_SUCCESS)
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Unable to allocate BUFFER HOST VMEM");
  }
  mem_alloc->m_vk_mem_sizes[E_GSVK_MEM_TYPE_BFF_HOST] = balloc_host_info.allocationSize;

  // Allocating image device memory
  if(vkAllocateMemory(m_vkrenderer.m_logical_device, 
                      &img_alloc_info, 
                      NULL, 
                      &mem_alloc->m_vk_mem_handles[E_GSVK_MEM_TYPE_IMG_DEVICE]) != VK_SUCCESS)
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, "Unable to allocate IMAGE DEVICE VMEM");
  }
  mem_alloc->m_vk_mem_sizes[E_GSVK_MEM_TYPE_IMG_DEVICE] = img_alloc_info.allocationSize;

  gs_pool_alloc_init(&mem_alloc->m_free_block_alloc, 
                     8, 
                     sizeof(gsvk_mem_free_block_t), 
                     GS_RENDERING_SMALL_PAGE_SIZE, 
                     &rendering_allocator.m_super);
}

void
gsvk_mem_alloc_release(gsvk_mem_alloc_t* mem_alloc)
{
  gsvk_mem_alloc_flush(mem_alloc);
  for(uint32_t mem_type = 0; mem_type < E_GSVK_MEM_NUM_TYPES; ++mem_type)
  {
    vkFreeMemory(m_vkrenderer.m_logical_device, 
                 mem_alloc->m_vk_mem_handles[mem_type], 
                 NULL);
  }
  
  gs_mutex_release(&mem_alloc->m_mutex);
  gs_pool_alloc_release(&mem_alloc->m_free_block_alloc);
}

void
gsvk_mem_alloc_flush(gsvk_mem_alloc_t* mem_alloc)
{
  gs_mutex_lock(&mem_alloc->m_mutex);
  for(uint32_t mem_type = 0; mem_type < E_GSVK_MEM_NUM_TYPES; ++mem_type)
  {
    gsvk_mem_free_block_t* next = mem_alloc->m_vk_mem_free_blocks[mem_type];
    while(next != NULL)
    {
      gsvk_mem_free_block_t* tmp = next->p_next;
      gs_pool_alloc_free(&mem_alloc->m_free_block_alloc, 
                         tmp);
      next = tmp;
    }
    mem_alloc->m_vk_mem_next_offset[mem_type] = 0;
  }
  gs_mutex_unlock(&mem_alloc->m_mutex);
}

void
gsvk_mem_alloc_alloc_no_lock(gsvk_mem_alloc_t* mem_alloc, 
                             VkMemoryRequirements* mem_reqs,
                             gsvk_mem_type_t mem_type, 
                             gsvk_mem_alloc_info_t* mem_info)
{
  if(!(mem_reqs->memoryTypeBits & (1 << mem_alloc->m_vk_mem_types[mem_type])))
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, "Invalid memory type %d for resource allocation", mem_type);
  }

  bool found = false;;
  if(mem_alloc->m_vk_mem_free_blocks[mem_type] != NULL)
  {
    // Check for valid free block
    gsvk_mem_free_block_t* prev = NULL;
    gsvk_mem_free_block_t* next = mem_alloc->m_vk_mem_free_blocks[mem_type];
    while(next != NULL && !found)
    {
      uint32_t offset = next->m_offset;
      uint32_t modulo = offset & (mem_reqs->alignment-1);
      uint32_t toadd = 0;
      if( modulo != 0)
      {
        toadd = (mem_reqs->alignment - modulo);
      }

      if(next->m_size >= (mem_reqs->size + toadd))
      {
        // we've found a suitable block
        mem_info->m_mhandle = mem_alloc->m_vk_mem_handles[mem_type];
        mem_info->m_vk_mem_type = mem_type;
        mem_info->m_offset = offset + toadd;
        mem_info->m_start = offset;
        mem_info->m_useful_size = mem_reqs->size;
        mem_info->m_real_size = next->m_size;
        found = true;
        if(prev == NULL)
        {
          mem_alloc->m_vk_mem_free_blocks[mem_type] = next->p_next;
        }
        else
        {
          prev->p_next = next->p_next;
        }
        gs_pool_alloc_free(&mem_alloc->m_free_block_alloc, 
                           next);
        break;
      }
      prev = next;
      next = next->p_next;
    }
  }

  if(!found)
  {
    // No free block was found
    uint64_t offset = mem_alloc->m_vk_mem_next_offset[mem_type];
    uint64_t modulo = offset & (mem_reqs->alignment-1);
    uint64_t toadd = 0;
    if( modulo != 0)
    {
      toadd = (mem_reqs->alignment - modulo);
    }
    if((offset + toadd + mem_reqs->size) > mem_alloc->m_vk_mem_sizes[mem_type])
    {
      GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, "Unable to allocate memory of type %d. Not enough memory", mem_type);
    }

    mem_info->m_mhandle = mem_alloc->m_vk_mem_handles[mem_type];
    mem_info->m_vk_mem_type = mem_type;
    mem_info->m_offset = offset + toadd;
    mem_info->m_start = offset;
    mem_info->m_useful_size = mem_reqs->size;
    mem_info->m_real_size = mem_reqs->size + toadd;

    mem_alloc->m_vk_mem_next_offset[mem_type] = offset + mem_info->m_real_size;
  }
}

void
gsvk_mem_alloc_buffer(gsvk_mem_alloc_t* mem_alloc, 
                      VkBuffer* buffer, 
                      gsvk_mem_type_t mem_type, 
                      gsvk_mem_alloc_info_t* mem_info)
{
  GS_ASSERT((mem_type == E_GSVK_MEM_TYPE_BFF_HOST ||
            mem_type == E_GSVK_MEM_TYPE_BFF_DEVICE) && "Invalid mem type for buffer");

  VkMemoryRequirements bmem_reqs;
  vkGetBufferMemoryRequirements(m_vkrenderer.m_logical_device, 
                                *buffer,
                                &bmem_reqs);

  gs_mutex_lock(&mem_alloc->m_mutex);
  gsvk_mem_alloc_alloc_no_lock(mem_alloc, 
                               &bmem_reqs,
                               mem_type,
                               mem_info);
  gs_mutex_unlock(&mem_alloc->m_mutex);
}

void
gsvk_mem_alloc_image(gsvk_mem_alloc_t* mem_alloc, 
                      VkImage* image, 
                      gsvk_mem_type_t mem_type, 
                      gsvk_mem_alloc_info_t* mem_info)
{
  GS_ASSERT(mem_type == E_GSVK_MEM_TYPE_IMG_DEVICE  && "Invalid mem type for image");

  VkMemoryRequirements imem_reqs;
  vkGetImageMemoryRequirements(m_vkrenderer.m_logical_device, 
                                *image,
                                &imem_reqs);
  gs_mutex_lock(&mem_alloc->m_mutex);
  gsvk_mem_alloc_alloc_no_lock(mem_alloc, 
                               &imem_reqs,
                               mem_type,
                               mem_info);
  gs_mutex_unlock(&mem_alloc->m_mutex);
}


void
gsvk_mem_alloc_free(gsvk_mem_alloc_t* mem_alloc, 
                    gsvk_mem_alloc_info_t* mem_info)
{
  gsvk_mem_free_block_t* free_block = gs_pool_alloc_alloc(&mem_alloc->m_free_block_alloc, 
                                                          8, 
                                                          sizeof(gsvk_mem_free_block_t), 
                                                          GS_NO_HINT);

  free_block->p_next = mem_alloc->m_vk_mem_free_blocks[mem_info->m_vk_mem_type];
  free_block->m_offset = mem_info->m_start;
  free_block->m_size = mem_info->m_real_size;
}
