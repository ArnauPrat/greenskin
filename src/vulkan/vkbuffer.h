

#ifndef _GS_VULKAN_BUFFER_H_
#define _GS_VULKAN_BUFFER_H_ 

#include "../platform.h"
#include <vulkan/vulkan.h>

typedef struct gsvk_buffer_t
{
  VkBuffer        m_bhandle;
  VkDeviceMemory  m_mhandle;
} gsvk_buffer_t;

typedef struct gsvk_vbuffer_t 
{
  gsvk_buffer_t   m_buffer;
  uint32_t        m_num_vertices;
} gsvk_vbuffer_t;

typedef struct gsvk_ibuffer_t 
{
  gsvk_buffer_t   m_buffer;
  uint32_t        m_num_indices;
} gsvk_ibuffer_t;

/**
 * \brief Creates a VKBuffer 
 *
 * \param size The size of the buffer to create
 * \param usage The usage flags of the buffer
 * \param properties The memory flags 
 * \param buffer The created buffer 
 * \param buffer_memory The created memory
 */
void 
gsvk_buffer_init(gsvk_buffer_t* buffer, 
                 VkDeviceSize size, 
                 VkBufferUsageFlags buffer_usage, 
                 VkMemoryPropertyFlagBits mem_props);


/**
 * \brief Releases a buffer
 *
 * \param buffer The buffer to release
 */
void 
gsvk_buffer_release(gsvk_buffer_t* buffer);

/**
 * \brief Copies one buffer into another
 *
 * \param dst The destination buffer
 * \param src The source buffer
 * \param device The device
 * \param queue The queue to use to perform the operation
 * \param command_pool The command pool to allocate the necessary commands
 * \param size The size to copy
 */
void
gsvk_copy_buffer(gsvk_buffer_t* dst,
                 gsvk_buffer_t* src,
                 VkDevice device,
                 VkQueue queue,
                 VkCommandPool command_pool,
                 VkDeviceSize size);

/**
 * \brief Creates a vertex buffer
 *
 * \param vertices The vertices array
 * \param buffer The created buffer
 * \param buffer_memory The created memory
 */
void
gsvk_vbuffer_init(gsvk_vbuffer_t* buffer, 
                  VkDevice device,
                  VkQueue queue,
                  VkCommandPool pool,
                  void* vertices, 
                  size_t size);

/**
 * \brief Releases a vertex buffer
 *
 * \param buffer The vertex buffer to release
 */
void
gsvk_vbuffer_release(gsvk_vbuffer_t* buffer);


/**
 * \brief Creates an index buffer
 *
 * \param vertices The indices array
 * \param buffer The created buffer
 * \param buffer_memory The created memory
 */
void 
gsvk_ibuffer_init(gsvk_ibuffer_t* buffer, 
                  VkDevice device,
                  VkQueue queue,
                  VkCommandPool pool,
                  void* indices, 
                  size_t size);

/**
 * \brief Releases an index buffer
 *
 * \param buffer The index buffer to release
 */
void
gsvk_ibuffer_release(gsvk_ibuffer_t* buffer);
  
#endif 
