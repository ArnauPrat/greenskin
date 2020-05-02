

#ifndef _GS_VKSWAPCHAIN_TOOLS_H_
#define _GS_VKSWAPCHAIN_TOOLS_H_

#include <vulkan/vulkan.h>

/**
 * \brief Given the different available surface formats for the swap chain,
 * choses the best one
 *
 * \return Returns the chosen surface format
 */
VkSurfaceFormatKHR 
gsvk_swapchain_sformat(void);

/**
 * \brief Given the different available present modes, choses the best one
 *
 *
 * \return The choosen present mode
 */
VkPresentModeKHR 
gsvk_swapchain_pmode(void);

/**
 * \brief Chooses the extent of the swap chain
 *
 * \param width The width of the viewport 
 * \param height The height of the viewport 
 *
 * \return Returns the choosen extent
 */
VkExtent2D 
gsvk_swapchain_extent(uint32_t width,
                      uint32_t height);

#endif
