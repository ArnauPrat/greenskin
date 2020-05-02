
#ifndef _GS_VKIMAGE_H_
#define _GS_VKIMAGE_H_

#include <vulkan/vulkan.h>

typedef struct gsvk_image_t
{
  VkImage         m_ihandle;
  VkDeviceMemory  m_mhandle;
  uint32_t        m_width;
  uint32_t        m_height;
} gsvk_image_t;

/**
 * \brief Creates a vulkan image
 *
 * \param memory_usage The type of memory
 * \param width The width of the image
 * \param height The height of the image
 * \param format The format of the image
 * \param tiling The tiling arrangement
 * \param usage The image usage flags
 */
void
gsvk_image_init(gsvk_image_t* image,
                uint32_t width, 
                uint32_t height, 
                VkFormat format, 
                VkImageTiling tiling, 
                VkImageUsageFlags usage, 
                VkMemoryPropertyFlagBits mem_props);

/**
 * \brief Releases the vulkan image
 *
 * \param image The image to release
 */
void
gsvk_image_release(gsvk_image_t* image);

/**
 * \brief Transitions the layoug of an image
 *
 * \param device The logical device to use
 * \param command_pool The command pool to use for the transition command
 * \param queue The queue to use for the transition command
 * \param image The image to transtion
 * \param format The format of the image
 * \param oldLayout The old image layout
 * \param newLayout The new image layout
 */
void
gsvk_image_transition_layout(gsvk_image_t* image, 
                             VkDevice device,
                             VkCommandPool command_pool,
                             VkQueue queue, 
                             VkFormat format, 
                             VkImageLayout oldLayout, 
                             VkImageLayout newLayout);

#endif 
