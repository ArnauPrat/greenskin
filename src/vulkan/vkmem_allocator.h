

#ifndef _GS_VKMEM_ALLOCATOR_H_
#define _GS_VKMEM_ALLOCATOR_H_ 

#include "../platform.h"
#include "../memory.h"
#include <vulkan/vulkan.h>
#include "../mutex.h"


typedef enum gsvk_mem_type_t
{
  E_GSVK_MEM_TYPE_BFF_HOST = 0,
  E_GSVK_MEM_TYPE_IMG_DEVICE = 1,
  E_GSVK_MEM_TYPE_BFF_DEVICE = 2,
  E_GSVK_MEM_NUM_TYPES 
} gsvk_mem_type_t;

typedef struct gsvk_mem_alloc_info_t
{
  VkDeviceMemory  m_mhandle;
  uint64_t        m_offset;
  uint64_t        m_start;
  uint64_t        m_useful_size;
  uint64_t        m_real_size;
  gsvk_mem_type_t m_vk_mem_type;
} gsvk_mem_alloc_info_t;

typedef struct gsvk_mem_free_block_t
{
  uint64_t                     m_offset;
  uint64_t                     m_size;
  struct gsvk_mem_free_block_t* p_next;
} gsvk_mem_free_block_t;

typedef struct gsvk_mem_alloc_t
{
  gs_mutex_t              m_mutex;
  uint32_t                m_vk_mem_types[E_GSVK_MEM_NUM_TYPES];
  uint32_t                m_vk_mem_heaps[E_GSVK_MEM_NUM_TYPES];
  VkDeviceMemory          m_vk_mem_handles[E_GSVK_MEM_NUM_TYPES];
  uint64_t                m_vk_mem_next_offset[E_GSVK_MEM_NUM_TYPES];
  uint64_t                m_vk_mem_sizes[E_GSVK_MEM_NUM_TYPES];
  gsvk_mem_free_block_t*  m_vk_mem_free_blocks[E_GSVK_MEM_NUM_TYPES];
  gs_pool_alloc_t         m_free_block_alloc;
} gsvk_mem_alloc_t;


/**
 * \brief Initializes the Vulkan memory allocator
 *
 * \param mem_alloc The vulkan memory allocator to initialize
 */
void
gsvk_mem_alloc_init(gsvk_mem_alloc_t* mem_alloc);

/**
 * \brief Releases the Vulkan memory allocator
 *
 * \param mem_alloc The vulkan memory allocator to release
 */
void
gsvk_mem_alloc_release(gsvk_mem_alloc_t* mem_alloc);

/**
 * \brief Flushes the allocations of the vulkan memory allocator
 *
 * \param mem_alloc The memory allocator
 */
void
gsvk_mem_alloc_flush(gsvk_mem_alloc_t* mem_alloc);

/**
 * \brief Allocates a chunk of memory for a buffer
 *
 * \param mem_alloc The vulkan memory allocator
 * \param mem_type The memory type for this buffer allocation
 * \param mem_info The returned allocation info
 */
void
gsvk_mem_alloc_buffer(gsvk_mem_alloc_t* mem_alloc, 
                      VkBuffer* buffer, 
                      gsvk_mem_type_t mem_type, 
                      gsvk_mem_alloc_info_t* mem_info);

/**
 * \brief Allocates a chunk of memory for an image 
 *
 * \param mem_alloc The vulkan memory allocator
 * \param mem_type The memory type for this image allocation
 * \param mem_info The returned allocation info
 */
void
gsvk_mem_alloc_image(gsvk_mem_alloc_t* mem_alloc, 
                      VkImage* image, 
                      gsvk_mem_type_t mem_type, 
                      gsvk_mem_alloc_info_t* mem_info);

/**
 * \brief Frees a chunk of memory
 *
 * \param mem_alloc The vulkan memory allocator
 * \param mem_info The memory allocation info to release
 */
void
gsvk_mem_alloc_free(gsvk_mem_alloc_t* mem_alloc, 
                    gsvk_mem_alloc_info_t* mem_info);


#endif 
