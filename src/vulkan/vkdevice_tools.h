

#ifndef _GS_VKDEVICE_H_
#define _GS_VKDEVICE_H_

#include "../platform.h"
#include <vulkan/vulkan.h>


/**
 * \brief Checks the support of the physical device for the desired extensions
 *
 * \param device The physical device to check the extensions support of
 * \param desired_extensions The extensions wanted to check
 *
 * \return Returns true if all the desired extensions are supported
 */
bool 
gsvk_check_device_extension_support(VkPhysicalDevice device,
                                    const char** device_extensions,
                                    uint32_t num_device_extensions);


/**
 * \brief Checks if the given physical device is suitable for our application
 *
 * \param device The physical device to check for
 * \param surface The rendering surface 
 *
 * \return Returns true if the device is suitable
 */
bool 
gsvk_is_device_suitable(VkPhysicalDevice device, 
                        VkSurfaceKHR surface,
                        int32_t gqueue_index,
                        int32_t pqueue_index,
                        const char** device_extensions,
                   uint32_t num_device_extensions); 
  
#endif
