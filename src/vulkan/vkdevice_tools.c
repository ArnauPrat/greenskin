


#include "../log.h"
#include "../platform.h"
#include "vkrenderer.h"
#include "vkdevice_tools.h"
#include "vkswapchain_tools.h"
#include <string.h>

bool 
gsvk_check_device_extension_support(VkPhysicalDevice device,
                                    const char** device_extensions,
                                    uint32_t num_device_extensions) 
{

    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    VkExtensionProperties* available_extensions = (VkExtensionProperties*) malloc(sizeof(VkExtensionProperties)*extension_count);

    vkEnumerateDeviceExtensionProperties(device, 
                                         NULL, 
                                         &extension_count, 
                                         available_extensions);

    GS_LOG_INFO("Checking physical device");
    bool all_found = true;
    for (uint32_t i = 0; i < num_device_extensions; ++i)
    {
      bool extension_found = false;
      const char* next_extension = device_extensions[i];
      GS_LOG_INFO("Looking for Vulkan device extension %s", next_extension);
      for (uint32_t j = 0; j < extension_count; ++j)
      {
        const char* available_extension = available_extensions[j].extensionName;
        if(strcmp(next_extension, available_extension) == 0)
        {
          GS_LOG_INFO("Found Vulkan device extension %s", available_extension);
          extension_found = true;
        }
      }
      all_found = all_found && extension_found;
    }

    free(available_extensions);
    return all_found;
}

bool 
gsvk_is_device_suitable(VkPhysicalDevice device, 
                        VkSurfaceKHR surface,
                        int32_t gqueue_index, 
                        int32_t pqueue_index, 
                        const char** device_extensions,
                        uint32_t num_device_extensions) 
{
  bool extensions_supported = gsvk_check_device_extension_support(device, 
                                                                  device_extensions,
                                                                  num_device_extensions);

  uint32_t num_sformats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, 
                                       surface, 
                                       &num_sformats, 
                                       NULL);

  uint32_t num_pmodes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, 
                                            surface, 
                                            &num_pmodes, 
                                            NULL);

  bool swap_chain_adequate = false;
  if(extensions_supported) 
  {
    swap_chain_adequate = num_sformats > 0 && num_pmodes > 0;
  }

  return ((gqueue_index != -1) && (pqueue_index != -1)) 
           && extensions_supported 
           && swap_chain_adequate;
}
