
#include "vkswapchain_tools.h"
#include "vkrenderer.h"
#include "../platform.h"
#include "../util.h"
#include <limits.h>




VkSurfaceFormatKHR 
gsvk_swapchain_sformat(void) 
{
  if (m_vkrenderer.m_num_pd_sformats == 1 && 
      m_vkrenderer.m_pd_sformats[0].format == VK_FORMAT_UNDEFINED) 
  {
    return (VkSurfaceFormatKHR){VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  for (uint32_t i = 0; i < m_vkrenderer.m_num_pd_sformats; ++i)
  {
    const VkSurfaceFormatKHR* format = &m_vkrenderer.m_pd_sformats[i];
    if (format->format == VK_FORMAT_B8G8R8A8_UNORM && 
        format->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
    {
      return *format;
    }
  }

  return m_vkrenderer.m_pd_sformats[0];
}

VkPresentModeKHR 
gsvk_swapchain_pmode(void) 
{
    VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < m_vkrenderer.m_num_pd_pmodes; ++i) 
    {
      VkPresentModeKHR mode = m_vkrenderer.m_pd_pmodes[i];
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR) 
      {
        return mode;
      } 
      else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) 
      {
            best_mode = mode;
      }
    }
    return best_mode;
}

VkExtent2D 
gsvk_swapchain_extent(uint32_t width,
                      uint32_t height) 
{
  if (m_vkrenderer.m_pd_scaps.currentExtent.width != UINT_MAX) 
  {
    return m_vkrenderer.m_pd_scaps.currentExtent;
  } 
  else 
  {
    VkExtent2D actual_extent = {width, height};

    actual_extent.width = MAX(m_vkrenderer.m_pd_scaps.minImageExtent.width, 
                              MIN(m_vkrenderer.m_pd_scaps.minImageExtent.width,
                                  actual_extent.width)
                             );

    actual_extent.height = MAX(m_vkrenderer.m_pd_scaps.minImageExtent.height, 
                               MIN(m_vkrenderer.m_pd_scaps.maxImageExtent.height, 
                                   actual_extent.height)
                              );

    return actual_extent;
  }
}
