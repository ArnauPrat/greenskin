

#include "../platform.h"
#include "../config.h"
#include "../matrix.h"
#include "../vertex.h"
#include "../log.h"
#include "../renderer.h"
#include "gui/vkgui.h"


#include "vkbuffer.h"
#include "vkdevice_tools.h"
#include "vkrenderer.h"
#include "vkshader_registry.h"
#include "vkswapchain_tools.h"
#include "vkvertex_tools.h"
#include "vkimage.h"
#include "vkpipeline.h"
#include <string.h>
#include <limits.h>


#define MAX_FRAME_BUFFERS 5
#define MAX_PRIMITIVE_COUNT 65536

// Forward declarations
void gsvk_vulkan_instance_init(void);
void gsvk_surface_init();
void gsvk_add_debug_handler(void);
void gsvk_physical_device_init(void);
void gsvk_logical_device_init(void);
void gsvk_command_pool_init(void);
void gsvk_swap_chain_init(void);
void gsvk_image_views_init(void);
void gsvk_descriptor_pool_init(void);
void gsvk_descriptor_set_init(void);
void gsvk_graphics_pipeline_init(void);
void gsvk_uniform_buffers_init(void);
void gsvk_command_buffers_init(void);
void gsvk_semaphores_init(void);
void gsvk_vulkan_instance_release(void);
void gsvk_surface_release();
void gsvk_add_debug_handler(void);
void gsvk_physical_device_release(void);
void gsvk_logical_device_release(void);
void gsvk_command_pool_release(void);
void gsvk_swap_chain_release(void);
void gsvk_image_views_release(void);
void gsvk_descriptor_pool_release(void);
void gsvk_descriptor_set_release(void);
void gsvk_graphics_pipeline_release(void);
void gsvk_uniform_buffers_release(void);
void gsvk_command_buffers_release(void);
void gsvk_semaphores_release(void);


/**
 * \brief Callback used to capture debug information
 *
 * \param flags
 * \param objType
 * \param obj
 * \param location
 * \param code
 * \param layerPrefix
 * \param msg
 * \param userData
 *
 * \return 
 */
VKAPI_ATTR VkBool32 VKAPI_CALL 
debug_callback(VkDebugReportFlagsEXT flags,
               VkDebugReportObjectTypeEXT objType,
               uint64_t obj,
               size_t location,
               int32_t code,
               const char* layerPrefix,
               const char* msg,
               void* userData) 
{
  GS_LOG_ERROR(E_RENDERER_VULKAN_ERROR, "validation layer: %s", msg);
  return VK_FALSE;
}

/**
 * \brief Flag used to enable validation layers or not
 */
#ifdef GS_DEBUG 
const bool m_enable_vlayers = true;
#else
const bool m_enable_vlayers = false;
#endif


gsvk_renderer_t                 m_vkrenderer; ///<  This is the VulkanRenderer

uint32_t                    m_num_uniform_buffers;
gsvk_buffer_t*              m_uniform_buffers;

void*                 m_udata = NULL;

void 
gsvk_renderer_init(const gs_config_t* config, 
                   GLFWwindow* window)
{
  GS_LOG_INFO("Initializing Vulkan Renderer");
  m_vkrenderer = (gsvk_renderer_t){0};
  gs_stack_alloc_init(&m_vkrenderer.m_stack_alloc, 
                      GS_RENDERING_LARGE_PAGE_SIZE, 
                      &rendering_allocator.m_super);
  gs_stack_alloc_init(&m_vkrenderer.m_sc_stack_alloc, 
                      GS_RENDERING_LARGE_PAGE_SIZE, 
                      &rendering_allocator.m_super);

  m_vkrenderer.p_window = NULL;
  m_vkrenderer.m_viewport_width = 0;                
  m_vkrenderer.m_viewport_height = 0;                
  m_vkrenderer.m_vulkan_instance = VK_NULL_HANDLE;   
  m_vkrenderer.m_window_surface = VK_NULL_HANDLE;   
  m_vkrenderer.m_report_callback = VK_NULL_HANDLE;   
  m_vkrenderer.m_physical_device = VK_NULL_HANDLE;
  m_vkrenderer.m_logical_device = VK_NULL_HANDLE;
  m_vkrenderer.m_swap_chain = VK_NULL_HANDLE;   
  m_vkrenderer.m_pipeline_layout = VK_NULL_HANDLE;  
  m_vkrenderer.m_render_pass = VK_NULL_HANDLE;  
  m_vkrenderer.m_pipeline = VK_NULL_HANDLE;  
  m_vkrenderer.m_command_pool = VK_NULL_HANDLE;  
  m_vkrenderer.m_descriptor_pool = VK_NULL_HANDLE; 
  m_vkrenderer.m_descriptor_set_layout  = VK_NULL_HANDLE; 
  m_vkrenderer.m_graphics_queue = VK_NULL_HANDLE; 
  m_vkrenderer.m_present_queue = VK_NULL_HANDLE;

  m_vkrenderer.m_pd_properties = (VkPhysicalDeviceProperties){0};
  m_vkrenderer.m_pd_mem_properties = (VkPhysicalDeviceMemoryProperties){0};
  m_vkrenderer.m_pd_scaps = (VkSurfaceCapabilitiesKHR){0};
  m_vkrenderer.m_pd_sformats = NULL;
  m_vkrenderer.m_num_pd_sformats = 0;
  m_vkrenderer.m_pd_pmodes = NULL;
  m_vkrenderer.m_num_pd_pmodes = 0;

  m_vkrenderer.m_surface_format = (VkSurfaceFormatKHR){0};
  m_vkrenderer.m_extent = (VkExtent2D){0};
  m_vkrenderer.m_gqueue_index = -1;
  m_vkrenderer.m_pqueue_index = -1;
  m_vkrenderer.m_depth_format = VK_FORMAT_UNDEFINED;
  m_vkrenderer.m_depth_image = (gsvk_image_t){VK_NULL_HANDLE, VK_NULL_HANDLE};
  m_vkrenderer.m_swap_chain_images = NULL;
  m_vkrenderer.m_swap_chain_image_views = NULL; 
  m_vkrenderer.m_frame_buffers = NULL;
  m_vkrenderer.m_command_buffers = NULL;
  m_vkrenderer.m_descriptor_sets = NULL;
  m_vkrenderer.m_validation_layers = NULL;  
  m_vkrenderer.m_device_extensions = NULL; 
  m_vkrenderer.m_num_swap_chain_images = 0;  
  m_vkrenderer.m_num_swap_chain_image_views = 0; 
  m_vkrenderer.m_num_validation_layers = 0;    
  m_vkrenderer.m_num_device_extensions = 0;   
  m_vkrenderer.m_num_frame_buffers = 0;   
  m_vkrenderer.m_num_command_buffers = 0;
  m_vkrenderer.m_num_descriptor_sets = 0;
  m_vkrenderer.m_image_available_semaphore = VK_NULL_HANDLE;
  m_vkrenderer.m_render_finished_semaphore = VK_NULL_HANDLE;
  m_vkrenderer.p_window = window;
  
  // Retrieving vulkan validation layers from engine config
  m_vkrenderer.m_num_validation_layers = config->m_nvkvls;
  m_vkrenderer.m_validation_layers = 
      (const char**) gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                          gs_os_min_alignment(), 
                                          sizeof(const char*)*m_vkrenderer.m_num_validation_layers, 
                                          GS_NO_HINT);
  for(uint32_t i = 0; i < m_vkrenderer.m_num_validation_layers; ++i) 
  {
    const char* layer = config->m_vkvls[i];
    m_vkrenderer.m_validation_layers[i] = 
              (char*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                          gs_os_min_alignment(),
                                          sizeof(char)*(strlen(layer)+1), 
                                          GS_NO_HINT);
    strcpy((char*)m_vkrenderer.m_validation_layers[i], layer);
  }

  // Retrieving viewport dimensions from engine config
  m_vkrenderer.m_viewport_width = config->m_vwidth;
  m_vkrenderer.m_viewport_height = config->m_vheight;

  if(m_enable_vlayers)
  {
    GS_LOG_WARNING("Vulkan validation layers enabled");
  }

  // Starting resource registries

  // Initializing the rest
  gsvk_vulkan_instance_init();
  gsvk_surface_init(window);
  gsvk_add_debug_handler();
  gsvk_physical_device_init();
  gsvk_logical_device_init();

  // Initializing Shader Registry
  gsvk_shader_registry_init();

  // Continue with the rest of initialization
  gsvk_command_pool_init();
  gsvk_swap_chain_init();
  gsvk_image_views_init();
  gsvk_descriptor_pool_init();
  gsvk_descriptor_set_init();
  gsvk_graphics_pipeline_init();
  gsvk_uniform_buffers_init();
  gsvk_command_buffers_init();
  gsvk_semaphores_init();

  return;
}

void
gsvk_renderer_release()
{
  GS_LOG_INFO("Releasing Vulkan Renderer");
  gsvk_semaphores_release();
  gsvk_uniform_buffers_release();
  gsvk_graphics_pipeline_release();
  gsvk_image_views_release();
  gsvk_swap_chain_release();
  gsvk_descriptor_set_release();
  gsvk_descriptor_pool_release();
  gsvk_command_pool_release();
  gsvk_command_buffers_release();
  gsvk_shader_registry_release();
  gsvk_logical_device_release();
  gsvk_physical_device_release();
  gsvk_surface_release();
  gsvk_vulkan_instance_release();

  if(m_vkrenderer.m_validation_layers)
  {
    for(uint32_t i = 0; i < m_vkrenderer.m_num_validation_layers; ++i)
    {
      gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, (void*)m_vkrenderer.m_validation_layers[i]);
    }
    gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, m_vkrenderer.m_validation_layers);
    m_vkrenderer.m_validation_layers = NULL;
    m_vkrenderer.m_num_validation_layers = 0; 
  }

  if(m_vkrenderer.m_device_extensions)
  {
    for(uint32_t i = 0; i < m_vkrenderer.m_num_device_extensions; ++i)
    {
      gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, (void*)m_vkrenderer.m_device_extensions[i]);
    }
    gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, m_vkrenderer.m_device_extensions);
    m_vkrenderer.m_device_extensions = NULL;
    m_vkrenderer.m_num_device_extensions = 0;
  }

  gs_stack_alloc_release(&m_vkrenderer.m_sc_stack_alloc);
  gs_stack_alloc_release(&m_vkrenderer.m_stack_alloc);
}


void 
gsvk_vulkan_instance_init() 
{
  GS_LOG_INFO("Initializing Vulkan instance");

  // Getting Available Validation Layers
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);
  VkLayerProperties* available_layers 
            = (VkLayerProperties*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                       gs_os_min_alignment(),
                                                       sizeof(VkLayerProperties)*layer_count, 
                                                       GS_NO_HINT);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers); 

  if(m_enable_vlayers && layer_count == 0)
  {
    GS_LOG_WARNING("Can't find validation layers. Is VK_LAYER_PATH variable set?");
  }

  for (uint32_t i = 0; i < m_vkrenderer.m_num_validation_layers; ++i) 
  {
    const char* layer = m_vkrenderer.m_validation_layers[i];
    bool layer_found = false;
    for (uint32_t j = 0; j < layer_count; ++j) 
    {
      VkLayerProperties* next_layer = &available_layers[j];
      if (strcmp(layer, next_layer->layerName) == 0) 
      {
        GS_LOG_INFO("Found validation layer %s", layer);
        layer_found = true;
        break;
      }
    }

    if (!layer_found) 
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                    "Validation layer %s not found", layer);
    }
  }
  gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, available_layers);

  // Creating Vulkan Application Information
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "TNA";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.pEngineName = "TNA Engine";
  app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.apiVersion = VK_API_VERSION_1_0;

  // Creating Vulkan Instance Information
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  if(m_enable_vlayers) 
  {
    create_info.enabledLayerCount = m_vkrenderer.m_num_validation_layers;
    create_info.ppEnabledLayerNames = m_vkrenderer.m_validation_layers;
  }

  // Adding GLFW and other required extensions
  unsigned int glfw_extensions_count = 0;
  const char** glfw_extensions 
                    = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
  
  uint32_t extensions_count = glfw_extensions_count;
  if(m_enable_vlayers) 
  {
    extensions_count += 1;
  }

  const char** extensions = 
      (const char**)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                         gs_os_min_alignment(),
                                         sizeof(char*)*extensions_count, 
                                         GS_NO_HINT);
  for(uint32_t i = 0; i < glfw_extensions_count; ++i) 
  {
    extensions[i] = glfw_extensions[i];
  }

  if(m_enable_vlayers) 
  {
    extensions[extensions_count - 1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  }

  create_info.enabledExtensionCount = extensions_count;
  create_info.ppEnabledExtensionNames = extensions;

  // Creating instance
  if(vkCreateInstance(&create_info, 
                      NULL, 
                      &m_vkrenderer.m_vulkan_instance) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to create instance!");
  }
  gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, 
                      extensions);
  return;
}

void
gsvk_surface_init() 
{
  GS_LOG_INFO("Initializing Vulkan surface");
  if (glfwCreateWindowSurface(m_vkrenderer.m_vulkan_instance, 
                              m_vkrenderer.p_window, 
                              NULL, 
                              &m_vkrenderer.m_window_surface) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to create window surface!");
  }
  return;
}

void
gsvk_add_debug_handler() 
{
  GS_LOG_INFO("Adding debug handler");

  // Adding debug callback
  if(m_enable_vlayers) {
    VkDebugReportCallbackCreateInfoEXT debug_callback_info = {};
    debug_callback_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_callback_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_callback_info.pfnCallback = debug_callback;

    PFN_vkCreateDebugReportCallbackEXT func
        = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(m_vkrenderer.m_vulkan_instance, 
                                                                     "vkCreateDebugReportCallbackEXT");
    if(func(m_vkrenderer.m_vulkan_instance, 
            &debug_callback_info, 
            NULL, 
            &m_vkrenderer.m_report_callback) != VK_SUCCESS) 
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                   "Failed to create debug callback!");
    }
  }
  return;
}

void
gsvk_physical_device_init() 
{
  GS_LOG_INFO("Initializing Vulkan physical device");

  // Adding the required device extensions
  m_vkrenderer.m_num_device_extensions = 1;
  m_vkrenderer.m_device_extensions  =
            (const char**)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                               gs_os_min_alignment(), 
                                               sizeof(const char*)*m_vkrenderer.m_num_device_extensions, 
                                               GS_NO_HINT);
  m_vkrenderer.m_device_extensions[0] = 
            (const char*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                              gs_os_min_alignment(), 
                                              sizeof(char)*(strlen(VK_KHR_SWAPCHAIN_EXTENSION_NAME)+1),
                                              GS_NO_HINT);
  strcpy((char*)m_vkrenderer.m_device_extensions[0], VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// Creating Physical Device
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(m_vkrenderer.m_vulkan_instance, 
                             &device_count, 
                             NULL);

  VkPhysicalDevice* physical_devices = 
            (VkPhysicalDevice*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                    gs_os_min_alignment(), 
                                                    sizeof(VkPhysicalDevice)*device_count, 
                                                    GS_NO_HINT);

  vkEnumeratePhysicalDevices(m_vkrenderer.m_vulkan_instance, 
                             &device_count, 
                             physical_devices);


  for (uint32_t i = 0; 
       i < device_count && m_vkrenderer.m_physical_device == VK_NULL_HANDLE; 
       ++i) 
  {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], 
                                             &queue_family_count, 
                                             NULL);

    VkQueueFamilyProperties * queue_families = 
              (VkQueueFamilyProperties*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                             gs_os_min_alignment(),
                                                             sizeof(VkQueueFamilyProperties)*queue_family_count, 
                                                             GS_NO_HINT);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], 
                                             &queue_family_count, 
                                             queue_families);

    for(uint32_t j = 0; j < queue_family_count; ++j)
    {
      VkQueueFamilyProperties* queue_family = &queue_families[j];
      if (queue_family->queueCount > 0 && queue_family->queueFlags & VK_QUEUE_GRAPHICS_BIT) 
      {
        m_vkrenderer.m_gqueue_index = j;
      }

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_devices[i], 
                                           j, 
                                           m_vkrenderer.m_window_surface, 
                                           &presentSupport);

      if(presentSupport) 
      {
        m_vkrenderer.m_pqueue_index = j;
      }
    }

    if (gsvk_is_device_suitable(physical_devices[i], 
                                m_vkrenderer.m_window_surface,
                                m_vkrenderer.m_gqueue_index,
                                m_vkrenderer.m_pqueue_index,
                                m_vkrenderer.m_device_extensions,
                                m_vkrenderer.m_num_device_extensions)) 
    {
      m_vkrenderer.m_physical_device = physical_devices[i];
    }
    else
    {
      m_vkrenderer.m_gqueue_index = -1;
      m_vkrenderer.m_pqueue_index = -1;
    }
    gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, 
                        queue_families);
  }
  gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, 
                      physical_devices);

  if (m_vkrenderer.m_physical_device == VK_NULL_HANDLE) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to find a suitable GPU!");
  }

  // Obtaining Physical device capbilities
  vkGetPhysicalDeviceProperties(m_vkrenderer.m_physical_device, &m_vkrenderer.m_pd_properties);
  vkGetPhysicalDeviceMemoryProperties(m_vkrenderer.m_physical_device, &m_vkrenderer.m_pd_mem_properties);
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkrenderer.m_physical_device, 
                                            m_vkrenderer.m_window_surface, 
                                            &m_vkrenderer.m_pd_scaps);

  vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkrenderer.m_physical_device, 
                                       m_vkrenderer.m_window_surface, 
                                       &m_vkrenderer.m_num_pd_sformats, 
                                       NULL);
  if (m_vkrenderer.m_num_pd_sformats != 0) 
  {
    m_vkrenderer.m_pd_sformats 
      = (VkSurfaceFormatKHR*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                  gs_os_min_alignment(), 
                                                  sizeof(VkSurfaceFormatKHR)*m_vkrenderer.m_num_pd_sformats, 
                                                  GS_NO_HINT);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkrenderer.m_physical_device, 
                                         m_vkrenderer.m_window_surface, 
                                         &m_vkrenderer.m_num_pd_sformats, 
                                         m_vkrenderer.m_pd_sformats);
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkrenderer.m_physical_device, 
                                            m_vkrenderer.m_window_surface, 
                                            &m_vkrenderer.m_num_pd_pmodes, 
                                            NULL);
  if(m_vkrenderer.m_num_pd_pmodes != 0)
  {
    m_vkrenderer.m_pd_pmodes 
      = (VkPresentModeKHR*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                gs_os_min_alignment(), 
                                                sizeof(VkPresentModeKHR)*m_vkrenderer.m_num_pd_pmodes, 
                                                GS_NO_HINT);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkrenderer.m_physical_device, 
                                              m_vkrenderer.m_window_surface, 
                                              &m_vkrenderer.m_num_pd_pmodes, 
                                              m_vkrenderer.m_pd_pmodes);
  }

  return;
}

void
gsvk_logical_device_init() 
{
  GS_LOG_INFO("Initializing Vulkan logical device");

  uint32_t num_distinct_queue_families = 1;
  if(m_vkrenderer.m_gqueue_index != 
     m_vkrenderer.m_pqueue_index)
  {
    ++num_distinct_queue_families;
  }

  // Creating Queue creation information 
  VkDeviceQueueCreateInfo* queue_create_infos 
            = (VkDeviceQueueCreateInfo*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                             gs_os_min_alignment(),
                                                             sizeof(VkDeviceQueueCreateInfo)*num_distinct_queue_families, 
                                                             GS_NO_HINT);

  float queue_priority = 1.0f;
  if(num_distinct_queue_families == 1)
  {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = m_vkrenderer.m_gqueue_index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos[0] = queue_create_info;
  }
  else
  {
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = m_vkrenderer.m_gqueue_index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos[0] = queue_create_info;

    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = m_vkrenderer.m_pqueue_index;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos[1] = queue_create_info;
  }

  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queue_create_infos;
  createInfo.queueCreateInfoCount = num_distinct_queue_families;
  createInfo.pEnabledFeatures = &device_features;
  createInfo.enabledExtensionCount = m_vkrenderer.m_num_device_extensions;
  createInfo.ppEnabledExtensionNames = m_vkrenderer.m_device_extensions;

  if (m_enable_vlayers) 
  {
    createInfo.enabledLayerCount = m_vkrenderer.m_num_validation_layers;
    createInfo.ppEnabledLayerNames = m_vkrenderer.m_validation_layers;
  } 
  else 
  {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(m_vkrenderer.m_physical_device, 
                     &createInfo, 
                     NULL, 
                     &m_vkrenderer.m_logical_device) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create logical device!");
  }

  gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, 
                      queue_create_infos);

  // Retrieving queues
  vkGetDeviceQueue(m_vkrenderer.m_logical_device, 
                   m_vkrenderer.m_gqueue_index, 
                   0, 
                   &m_vkrenderer.m_graphics_queue);

  vkGetDeviceQueue(m_vkrenderer.m_logical_device, 
                   m_vkrenderer.m_pqueue_index, 
                   0, 
                   &m_vkrenderer.m_present_queue);
  return;
}

void
gsvk_swap_chain_init() 
{
  GS_LOG_INFO("Initializing Vulkan swap chain");

  m_vkrenderer.m_surface_format = gsvk_swapchain_sformat();

  VkPresentModeKHR present_mode = gsvk_swapchain_pmode();

  m_vkrenderer.m_extent = gsvk_swapchain_extent(m_vkrenderer.m_viewport_width, 
                                                m_vkrenderer.m_viewport_height);

  uint32_t image_count = m_vkrenderer.m_pd_scaps.minImageCount + 1;

  if (m_vkrenderer.m_pd_scaps.maxImageCount > 0 && 
      image_count > m_vkrenderer.m_pd_scaps.maxImageCount) 
  {
    image_count = m_vkrenderer.m_pd_scaps.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swap_chain_create_info = {};
  swap_chain_create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_create_info.surface          = m_vkrenderer.m_window_surface;
  swap_chain_create_info.minImageCount    = image_count;
  swap_chain_create_info.imageFormat      = m_vkrenderer.m_surface_format.format;
  swap_chain_create_info.imageColorSpace  = m_vkrenderer.m_surface_format.colorSpace;
  swap_chain_create_info.imageExtent      = m_vkrenderer.m_extent;
  swap_chain_create_info.imageArrayLayers = 1;
  swap_chain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queue_family_indices[] = {(uint32_t)m_vkrenderer.m_gqueue_index, 
                                    (uint32_t) m_vkrenderer.m_pqueue_index};

  if (m_vkrenderer.m_gqueue_index != m_vkrenderer.m_pqueue_index) 
  {
    swap_chain_create_info.imageSharingMode       = VK_SHARING_MODE_CONCURRENT;
    swap_chain_create_info.queueFamilyIndexCount  = 2;
    swap_chain_create_info.pQueueFamilyIndices    = queue_family_indices;
  } 
  else 
  {
    swap_chain_create_info.imageSharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    swap_chain_create_info.queueFamilyIndexCount  = 0; // Optional
    swap_chain_create_info.pQueueFamilyIndices    = NULL; // Optional
  }

  swap_chain_create_info.preTransform   = m_vkrenderer.m_pd_scaps.currentTransform;
  swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_create_info.presentMode    = present_mode;
  swap_chain_create_info.clipped        = VK_TRUE;

  swap_chain_create_info.oldSwapchain   = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_vkrenderer.m_logical_device, 
                           &swap_chain_create_info, 
                           NULL, 
                           &m_vkrenderer.m_swap_chain) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR,
                  "failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(m_vkrenderer.m_logical_device, 
                          m_vkrenderer.m_swap_chain, 
                          &m_vkrenderer.m_num_swap_chain_images, 
                          NULL);

  if(m_vkrenderer.m_num_swap_chain_images > MAX_FRAME_BUFFERS)
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "The number of swapchain images exceeds the limit: %d", m_vkrenderer.m_num_swap_chain_images);
  }

  m_vkrenderer.m_swap_chain_images = 
              (VkImage*) gs_stack_alloc_alloc(&m_vkrenderer.m_sc_stack_alloc, 
                                              gs_os_min_alignment(), 
                                              sizeof(VkImage)*m_vkrenderer.m_num_swap_chain_images, 
                                              GS_NO_HINT);
  vkGetSwapchainImagesKHR(m_vkrenderer.m_logical_device, 
                          m_vkrenderer.m_swap_chain, 
                          &m_vkrenderer.m_num_swap_chain_images, 
                          m_vkrenderer.m_swap_chain_images);

  /// We create the depth buffer image
  VkFormat candidates[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  m_vkrenderer.m_depth_format = gsvk_find_supported_format(m_vkrenderer.m_physical_device,
                                                           candidates, 
                                                           sizeof(candidates),
                                                           VK_IMAGE_TILING_OPTIMAL,
                                                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

  if (m_vkrenderer.m_depth_format == VK_FORMAT_UNDEFINED) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to obtain the depth buffer format!");
  }

  gsvk_image_init(&m_vkrenderer.m_depth_image, 
                  m_vkrenderer.m_viewport_width, 
                  m_vkrenderer.m_viewport_height, 
                  m_vkrenderer.m_depth_format, 
                  VK_IMAGE_TILING_OPTIMAL, 
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  gsvk_image_transition_layout(&m_vkrenderer.m_depth_image, 
                               m_vkrenderer.m_logical_device,
                               m_vkrenderer.m_command_pool,
                               m_vkrenderer.m_graphics_queue,
                               m_vkrenderer.m_depth_format, 
                               VK_IMAGE_LAYOUT_UNDEFINED, 
                               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  return;
}

void
gsvk_image_views_init(void) 
{
  GS_LOG_INFO("Initializing Vulkan image views");

  m_vkrenderer.m_num_swap_chain_image_views = m_vkrenderer.m_num_swap_chain_images;
  m_vkrenderer.m_swap_chain_image_views = 
            (VkImageView*) gs_stack_alloc_alloc(&m_vkrenderer.m_sc_stack_alloc, 
                                                gs_os_min_alignment(), 
                                                sizeof(VkImageView)*m_vkrenderer.m_num_swap_chain_image_views, 
                                                GS_NO_HINT);

  for(size_t i = 0; i < m_vkrenderer.m_num_swap_chain_image_views; ++i) 
  {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = m_vkrenderer.m_swap_chain_images[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = m_vkrenderer.m_surface_format.format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_vkrenderer.m_logical_device, 
                          &create_info, 
                          NULL, 
                          &m_vkrenderer.m_swap_chain_image_views[i]) != VK_SUCCESS) 
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR,
                    "failed to create image views!");
    }
  }


    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = m_vkrenderer.m_depth_image.m_ihandle;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = m_vkrenderer.m_depth_format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_vkrenderer.m_logical_device, 
                          &create_info, 
                          NULL, 
                          &m_vkrenderer.m_depth_image_view) != VK_SUCCESS) 
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                    "failed to create depth image view!");
    }



  return;
}

void
gsvk_graphics_pipeline_init() 
{
  GS_LOG_INFO("Initializing Vulkan pipeline");

  VkShaderModule* vertex_shader = gsvk_shader_registry_load("shaders/shader.vert.spv");
  VkShaderModule* fragment_shader = gsvk_shader_registry_load("shaders/shader.frag.spv");
  
  if(!vertex_shader) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "Vertex shader not found");
  }

  if(!fragment_shader) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "Fragment shader not found");
  }

  VkPipelineShaderStageCreateInfo vertex_shader_stage = gsvk_build_vertex_shader_stage(vertex_shader);

  VkPipelineShaderStageCreateInfo fragment_shader_stage = gsvk_build_fragment_shader_stage(fragment_shader);

  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage, fragment_shader_stage};

  VkVertexInputBindingDescription binding_description = get_binding_description();
  VkVertexInputAttributeDescription input_attr_desc[3];
  get_attribute_descriptions(input_attr_desc);

  VkPipelineVertexInputStateCreateInfo vertex_infput_info = {};
  vertex_infput_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_infput_info.vertexBindingDescriptionCount = 1;
  vertex_infput_info.pVertexBindingDescriptions = &binding_description; // Optional
  vertex_infput_info.vertexAttributeDescriptionCount = 3;
  vertex_infput_info.pVertexAttributeDescriptions = input_attr_desc; // Optional

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) m_vkrenderer.m_extent.width;
  viewport.height = (float) m_vkrenderer.m_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = (VkOffset2D){0, 0};
  scissor.extent = m_vkrenderer.m_extent;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f; // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = NULL; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
  depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state.depthTestEnable = VK_TRUE;
  depth_stencil_state.depthWriteEnable = VK_TRUE;
  depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_state.stencilTestEnable = VK_FALSE;
  depth_stencil_state.pNext = NULL;
  depth_stencil_state.flags = 0;
  depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
  depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
  depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
  depth_stencil_state.back.compareMask = 0;
  depth_stencil_state.back.reference = 0;
  depth_stencil_state.back.depthFailOp = VK_STENCIL_OP_KEEP;
  depth_stencil_state.back.writeMask = 0;
  depth_stencil_state.front = depth_stencil_state.back;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f; // Optional
  color_blending.blendConstants[1] = 0.0f; // Optional
  color_blending.blendConstants[2] = 0.0f; // Optional
  color_blending.blendConstants[3] = 0.0f; // Optional

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1; // Optional
  pipeline_layout_info.pSetLayouts = &m_vkrenderer.m_descriptor_set_layout; // Optional
  pipeline_layout_info.pushConstantRangeCount = 0; // Optional
  pipeline_layout_info.pPushConstantRanges = NULL; // Optional

  if (vkCreatePipelineLayout(m_vkrenderer.m_logical_device, 
                             &pipeline_layout_info, 
                             NULL, 
                             &m_vkrenderer.m_pipeline_layout) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to create pipeline layout!");
  }

  VkAttachmentDescription color_attachment;
  color_attachment.format = m_vkrenderer.m_surface_format.format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  color_attachment.flags = 0;

  VkAttachmentDescription depth_attachment;
  depth_attachment.format = m_vkrenderer.m_depth_format;
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depth_attachment.flags = 0;

  VkAttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[] = {color_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = sizeof(attachments) / sizeof(VkAttachmentDescription);
  render_pass_info.pAttachments = &attachments[0];
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(m_vkrenderer.m_logical_device, 
                         &render_pass_info, 
                         NULL, 
                         &m_vkrenderer.m_render_pass) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create render pass!");
  }

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_infput_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = &depth_stencil_state; // Optional
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = NULL; // Optional
  pipeline_info.layout = m_vkrenderer.m_pipeline_layout;
  pipeline_info.renderPass = m_vkrenderer.m_render_pass;
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipeline_info.basePipelineIndex = -1; // Optional
  pipeline_info.pNext = NULL;

  if (vkCreateGraphicsPipelines(m_vkrenderer.m_logical_device, 
                                VK_NULL_HANDLE, 
                                1, 
                                &pipeline_info, 
                                NULL, 
                                &m_vkrenderer.m_pipeline) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create graphics pipeline!");
  }

  m_vkrenderer.m_num_frame_buffers = m_vkrenderer.m_num_swap_chain_image_views;
  m_vkrenderer.m_frame_buffers = 
              (VkFramebuffer*) gs_stack_alloc_alloc(&m_vkrenderer.m_sc_stack_alloc, 
                                                    gs_os_min_alignment(),
                                                    sizeof(VkFramebuffer)*m_vkrenderer.m_num_frame_buffers, 
                                                    GS_NO_HINT);

  for (size_t i = 0; i < m_vkrenderer.m_num_frame_buffers; i++) 
  {
    VkImageView attachments[] = {
      m_vkrenderer.m_swap_chain_image_views[i],
      m_vkrenderer.m_depth_image_view
    };


    VkFramebufferCreateInfo frame_buffer_info = {};
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_info.renderPass = m_vkrenderer.m_render_pass;
    frame_buffer_info.attachmentCount = sizeof(attachments) / sizeof(VkImageView);
    frame_buffer_info.pAttachments = &attachments[0];
    frame_buffer_info.width = m_vkrenderer.m_extent.width;
    frame_buffer_info.height = m_vkrenderer.m_extent.height;
    frame_buffer_info.layers = 1;

    if (vkCreateFramebuffer(m_vkrenderer.m_logical_device, 
                            &frame_buffer_info, 
                            NULL, 
                            &m_vkrenderer.m_frame_buffers[i]) != VK_SUCCESS) 
    {
      GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                   "failed to create framebuffer!");
    }
  }

  return;
}

void
gsvk_command_pool_init() 
{
  GS_LOG_INFO("Initializing Vulkan command pool");

  VkCommandPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = m_vkrenderer.m_gqueue_index;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

  if (vkCreateCommandPool(m_vkrenderer.m_logical_device, 
                          &pool_info, 
                          NULL, 
                          &m_vkrenderer.m_command_pool) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create command pool!");
  }
  return;
}

void 
gsvk_descriptor_pool_init() 
{
  GS_LOG_INFO("Initializing Vulkan command pool");

  /*VkDescriptorPoolSize pool_size = {};
  pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  pool_size.descriptorCount = m_vkrenderer.m_num_swap_chain_images;
  */

  uint32_t num_descriptors = 1000*m_vkrenderer.m_num_swap_chain_images;

  VkDescriptorPoolSize pool_sizes[] =
  {
    { VK_DESCRIPTOR_TYPE_SAMPLER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, num_descriptors },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, num_descriptors },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, num_descriptors },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, num_descriptors },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, num_descriptors },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, num_descriptors }
  };

  size_t pool_sizes_count = sizeof(pool_sizes) / sizeof(VkDescriptorPoolSize);

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.poolSizeCount = pool_sizes_count;
  pool_info.pPoolSizes = pool_sizes;
  pool_info.maxSets = num_descriptors;

  if (vkCreateDescriptorPool(m_vkrenderer.m_logical_device, 
                             &pool_info, 
                             NULL, 
                             &m_vkrenderer.m_descriptor_pool) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create descriptor pool!");
  }
  return;
}

void
gsvk_descriptor_set_init() 
{
  GS_LOG_INFO("Initializing Vulkan descriptor sets");

  VkDescriptorSetLayoutBinding ubo_layout_info = {};
  ubo_layout_info.binding = 0;
  ubo_layout_info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  ubo_layout_info.descriptorCount = 1;
  ubo_layout_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  ubo_layout_info.pImmutableSamplers = NULL; // Optional

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 1;
  layout_info.pBindings = &ubo_layout_info;

  if (vkCreateDescriptorSetLayout(m_vkrenderer.m_logical_device, 
                                  &layout_info, 
                                  NULL, 
                                  &m_vkrenderer.m_descriptor_set_layout) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to create descriptor set layout!");
  }

  m_vkrenderer.m_num_descriptor_sets = m_vkrenderer.m_num_swap_chain_images;
  m_vkrenderer.m_descriptor_sets = 
              (VkDescriptorSet*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                     gs_os_min_alignment(), 
                                                     sizeof(VkDescriptorSet)*m_vkrenderer.m_num_descriptor_sets, 
                                                     GS_NO_HINT);

  VkDescriptorSetLayout* layouts = 
              (VkDescriptorSetLayout*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                           gs_os_min_alignment(), 
                                                           sizeof(VkDescriptorSetLayout)*m_vkrenderer.m_num_descriptor_sets, 
                                                           GS_NO_HINT);
  for(uint32_t i = 0; i < m_vkrenderer.m_num_descriptor_sets; ++i)
  {
    layouts[i] = m_vkrenderer.m_descriptor_set_layout;
  }

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_vkrenderer.m_descriptor_pool;
  allocInfo.descriptorSetCount = m_vkrenderer.m_num_descriptor_sets;
  allocInfo.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(m_vkrenderer.m_logical_device, 
                               &allocInfo, 
                               &m_vkrenderer.m_descriptor_sets[0]) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to allocate descriptor set!");
  }

  gs_stack_alloc_pop(&m_vkrenderer.m_stack_alloc, 
                      layouts);
  return;
}

void 
gsvk_uniform_buffers_init() 
{
  GS_LOG_INFO("Initializing Vulkan uniform buffers");

  size_t device_alignment = m_vkrenderer.m_pd_properties.limits.minUniformBufferOffsetAlignment;
  size_t uniform_buffer_size = sizeof(gs_render_mesh_uniform_t);
  size_t dynamic_alignment = (uniform_buffer_size / device_alignment) * device_alignment + ((uniform_buffer_size % device_alignment) > 0 ? device_alignment : 0);

  size_t bufferSize = dynamic_alignment * MAX_PRIMITIVE_COUNT;


  m_num_uniform_buffers = m_vkrenderer.m_num_swap_chain_images;
  m_uniform_buffers = (gsvk_buffer_t*)gs_stack_alloc_alloc(&m_vkrenderer.m_stack_alloc, 
                                                           gs_os_min_alignment(), 
                                                           sizeof(gsvk_buffer_t)*m_num_uniform_buffers, 
                                                           GS_NO_HINT);

  for(uint32_t i = 0; i < m_num_uniform_buffers; ++i)
  {
    gsvk_buffer_init(&m_uniform_buffers[i],
                     bufferSize, 
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = m_uniform_buffers[i].m_bhandle;
    buffer_info.offset = 0;
    buffer_info.range = dynamic_alignment;

    VkWriteDescriptorSet descriptor_write = {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.pNext = NULL;
    descriptor_write.dstSet = m_vkrenderer.m_descriptor_sets[i];
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;

    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = NULL; // Optional
    descriptor_write.pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(m_vkrenderer.m_logical_device, 
                           1, 
                           &descriptor_write, 
                           0, 
                           NULL);

  }
  return;
}

void  
gsvk_command_buffers_init(void) 
{
  m_vkrenderer.m_num_command_buffers = m_vkrenderer.m_num_frame_buffers;
  m_vkrenderer.m_command_buffers = 
  (VkCommandBuffer*) gs_stack_alloc_alloc(&m_vkrenderer.m_sc_stack_alloc, 
                                          gs_os_min_alignment(), 
                                          sizeof(VkCommandBuffer)*m_vkrenderer.m_num_command_buffers, 
                                          GS_NO_HINT);

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_vkrenderer.m_command_pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t) m_vkrenderer.m_num_command_buffers;

  if (vkAllocateCommandBuffers(m_vkrenderer.m_logical_device, 
                               &allocInfo, 
                               m_vkrenderer.m_command_buffers) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                 "failed to allocate command buffers!");
  }
  return;
}

void 
gsvk_semaphores_init(void) 
{
  GS_LOG_INFO("Initializing Vulkan semaphores");

  VkSemaphoreCreateInfo semaphore_info = {};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_vkrenderer.m_logical_device, 
                        &semaphore_info, 
                        NULL, 
                        &m_vkrenderer.m_image_available_semaphore) != VK_SUCCESS ||
			vkCreateSemaphore(m_vkrenderer.m_logical_device, 
                        &semaphore_info, 
                        NULL, 
                        &m_vkrenderer.m_render_finished_semaphore) != VK_SUCCESS) 
  {
		GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "failed to create semaphores!");
	}
  return;
}


void 
gsvk_swap_chain_release(void) 
{
  GS_LOG_INFO("Releasing Vulkan swap chain");

  gsvk_image_release(&m_vkrenderer.m_depth_image);

  if(m_vkrenderer.m_swap_chain_images)
  {
    gs_stack_alloc_free(&m_vkrenderer.m_sc_stack_alloc, 
                        m_vkrenderer.m_swap_chain_images);
    m_vkrenderer.m_swap_chain_images = NULL;
    m_vkrenderer.m_num_swap_chain_images = 0;
  }

  vkDestroySwapchainKHR(m_vkrenderer.m_logical_device, 
                        m_vkrenderer.m_swap_chain, 
                        NULL);

  return;
}

void
gsvk_image_views_release(void)
{
  GS_LOG_INFO("Releasing Vulkan image views");

  vkDestroyImageView(m_vkrenderer.m_logical_device, 
                     m_vkrenderer.m_depth_image_view, 
                     NULL);

  if(m_vkrenderer.m_swap_chain_image_views)
  {
    for(uint32_t i = 0; i < m_vkrenderer.m_num_swap_chain_image_views; ++i)
    {
      vkDestroyImageView(m_vkrenderer.m_logical_device, 
                         m_vkrenderer.m_swap_chain_image_views[i], 
                         NULL);
    }
    gs_stack_alloc_free(&m_vkrenderer.m_sc_stack_alloc, 
                        m_vkrenderer.m_swap_chain_image_views);
    m_vkrenderer.m_swap_chain_image_views = NULL;
    m_vkrenderer.m_num_swap_chain_image_views = 0;
  }
  return;
}

void
gsvk_graphics_pipeline_release()
{
  GS_LOG_INFO("Releasing Vulkan graphics pipeline");

  if(m_vkrenderer.m_frame_buffers)
  {
    for (uint32_t i = 0; i < m_vkrenderer.m_num_frame_buffers; ++i) 
    {
      vkDestroyFramebuffer(m_vkrenderer.m_logical_device, 
                           m_vkrenderer.m_frame_buffers[i], 
                           NULL);
    }
    gs_stack_alloc_free(&m_vkrenderer.m_sc_stack_alloc, 
                        m_vkrenderer.m_frame_buffers);
    m_vkrenderer.m_frame_buffers = NULL;
    m_vkrenderer.m_num_frame_buffers = 0;
  }

  vkDestroyPipeline(m_vkrenderer.m_logical_device, m_vkrenderer.m_pipeline, NULL);
  vkDestroyRenderPass(m_vkrenderer.m_logical_device, m_vkrenderer.m_render_pass, NULL);
  vkDestroyPipelineLayout(m_vkrenderer.m_logical_device, m_vkrenderer.m_pipeline_layout, NULL);
  return;
}

void
gsvk_semaphores_release()
{
  GS_LOG_INFO("Releasing Vulkan semaphores");

  if(m_vkrenderer.m_render_finished_semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_vkrenderer.m_logical_device, 
                       m_vkrenderer.m_render_finished_semaphore, 
                       NULL);
    m_vkrenderer.m_render_finished_semaphore = VK_NULL_HANDLE;
  }

  if(m_vkrenderer.m_image_available_semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_vkrenderer.m_logical_device, 
                       m_vkrenderer.m_image_available_semaphore, 
                       NULL);
    m_vkrenderer.m_image_available_semaphore = VK_NULL_HANDLE;
  }

}

void
gsvk_descriptor_set_release()
{
  GS_LOG_INFO("Releasing Vulkan descriptor sets");

  if(m_vkrenderer.m_descriptor_sets)
  {
    gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, 
                        m_vkrenderer.m_descriptor_sets);
    m_vkrenderer.m_descriptor_sets = NULL;
    m_vkrenderer.m_num_descriptor_sets = 0;
  }

  if(m_vkrenderer.m_descriptor_set_layout != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(m_vkrenderer.m_logical_device, 
                                 m_vkrenderer.m_descriptor_set_layout, 
                                 NULL);
    m_vkrenderer.m_descriptor_set_layout = VK_NULL_HANDLE;
  }
}


void
gsvk_descriptor_pool_release()
{
  GS_LOG_INFO("Releasing Vulkan descriptor pool");

  if(m_vkrenderer.m_descriptor_pool != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(m_vkrenderer.m_logical_device, 
                            m_vkrenderer.m_descriptor_pool, 
                            NULL);
    m_vkrenderer.m_descriptor_pool = VK_NULL_HANDLE;
  }
}

void
gsvk_command_pool_release()
{
  GS_LOG_INFO("Releasing Vulkan command pool");

  if(m_vkrenderer.m_command_pool != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(m_vkrenderer.m_logical_device, 
                         m_vkrenderer.m_command_pool, 
                         NULL);
    m_vkrenderer.m_command_pool = VK_NULL_HANDLE;
  }

}

void
gsvk_command_buffers_release()
{
  GS_LOG_INFO("Releasing Vulkan command buffers");

  if(m_vkrenderer.m_command_buffers)
  {
    gs_stack_alloc_free(&m_vkrenderer.m_sc_stack_alloc, 
                        m_vkrenderer.m_command_buffers);
    m_vkrenderer.m_command_buffers = NULL;;
    m_vkrenderer.m_num_command_buffers = 0;
  }
}

void
gsvk_logical_device_release()
{
  GS_LOG_INFO("Releasing Vulkan logical device");

  if(m_vkrenderer.m_logical_device != VK_NULL_HANDLE)
  {
    vkDestroyDevice(m_vkrenderer.m_logical_device, 
                    NULL);
    m_vkrenderer.m_logical_device = VK_NULL_HANDLE;
  }
}

void
gsvk_physical_device_release()
{
  GS_LOG_INFO("Releasing Vulkan physical device");

  if(m_vkrenderer.m_pd_sformats != NULL)
  {
    gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, 
                        m_vkrenderer.m_pd_sformats);
  }

  if(m_vkrenderer.m_pd_pmodes != NULL)
  {
    gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, 
                        m_vkrenderer.m_pd_pmodes);
  }
}

void
gsvk_surface_release()
{
  GS_LOG_INFO("Releasing Vulkan surface");

  if(m_vkrenderer.m_window_surface != VK_NULL_HANDLE)
  {
    vkDestroySurfaceKHR(m_vkrenderer.m_vulkan_instance, 
                        m_vkrenderer.m_window_surface, 
                        NULL);
    m_vkrenderer.m_window_surface = VK_NULL_HANDLE;
  }
}

void
gsvk_vulkan_instance_release()
{
  GS_LOG_INFO("Releasing Vulkan instance");

  if (m_enable_vlayers) 
  {
    PFN_vkDestroyDebugReportCallbackEXT func 
    = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(m_vkrenderer.m_vulkan_instance, 
                                                                  "vkDestroyDebugReportCallbackEXT");
    func(m_vkrenderer.m_vulkan_instance, 
         m_vkrenderer.m_report_callback, 
         NULL);
  }

  if(m_vkrenderer.m_vulkan_instance != VK_NULL_HANDLE)
  {
    vkDestroyInstance(m_vkrenderer.m_vulkan_instance, NULL);
    m_vkrenderer.m_vulkan_instance = VK_NULL_HANDLE;
  }
}

void
gsvk_uniform_buffers_release()
{
  GS_LOG_INFO("Releasing Vulkan uniform buffers");

  vkDeviceWaitIdle(m_vkrenderer.m_logical_device);

  for(uint32_t i = 0; i < m_num_uniform_buffers; ++i)
  {
    gsvk_buffer_release(&m_uniform_buffers[i]);
  }
  gs_stack_alloc_free(&m_vkrenderer.m_stack_alloc, 
                      m_uniform_buffers);
}

void 
gsvk_recreate_swap_chain() 
{
  vkDeviceWaitIdle(m_vkrenderer.m_logical_device);

  gsvk_command_buffers_release();
  gsvk_graphics_pipeline_release();
  gsvk_image_views_release();
  gsvk_swap_chain_release();

  gs_stack_alloc_release(&m_vkrenderer.m_sc_stack_alloc);
  gs_stack_alloc_init(&m_vkrenderer.m_sc_stack_alloc, 
                      GS_RENDERING_LARGE_PAGE_SIZE, 
                      &rendering_allocator.m_super);

  gsvk_swap_chain_init();
  gsvk_image_views_init();
  gsvk_graphics_pipeline_init();
  gsvk_command_buffers_init();
  return;
}

void 
gsvk_build_command_buffer(uint32_t index, gs_rendering_scene_t* scene) 
{
  //size_t device_alignment = m_vkrenderer.m_pd_properties.limits.minUniformBufferOffsetAlignment;
  //size_t uniform_buffer_size = sizeof(gs_render_mesh_uniform_t);
  //size_t dynamic_alignment = (uniform_buffer_size / device_alignment) * device_alignment + ((uniform_buffer_size % device_alignment) > 0 ? device_alignment : 0);

  /*
  uint32_t num_meshes = scene->m_meshes.size();
  if( num_meshes > 0)
  {
    matrix4_t proj_view_mat = scene->m_proj_mat * scene->m_view_mat;
    char* uniform_data = NULL;
    vmaMapMemory(m_vkrenderer.m_vkallocator, m_uniform_allocations[index], (void**)&uniform_data);
    for(size_t i = 0; (i < num_meshes) && (i < MAX_PRIMITIVE_COUNT); ++i) 
    {
      render_mesh_uniform_t* uniform = (render_mesh_uniform_t*)(&scene->m_uniforms[i]);
      render_mesh_uniform_t* dst_uniform = (render_mesh_uniform_t*)(&uniform_data[dynamic_alignment*i]);
      dst_uniform->m_model_matrix =  proj_view_mat * uniform->m_model_matrix;
      dst_uniform->m_material = uniform->m_material;
    }

    vmaUnmapMemory(m_vkrenderer.m_vkallocator, m_uniform_allocations[index]);
  }
  */

  /// Setting up command buffer
  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  begin_info.pInheritanceInfo = NULL; // Optional

  if (vkBeginCommandBuffer(m_vkrenderer.m_command_buffers[index], &begin_info) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, 
                  "failed to begin recording command buffer!");
  }

  vkCmdBindPipeline(m_vkrenderer.m_command_buffers[index], 
                    VK_PIPELINE_BIND_POINT_GRAPHICS, 
                    m_vkrenderer.m_pipeline);

  VkRenderPassBeginInfo renderpass_info = {};
  renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderpass_info.renderPass = m_vkrenderer.m_render_pass;
  renderpass_info.framebuffer = m_vkrenderer.m_frame_buffers[index];
  renderpass_info.renderArea.offset = (VkOffset2D){0, 0};
  renderpass_info.renderArea.extent = m_vkrenderer.m_extent;


  VkClearValue clear_values[2];
  clear_values[0].color = (VkClearColorValue){{0.0, 0.0, 0.2, 1.0f}};
  clear_values[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};
  renderpass_info.clearValueCount = 2;
  renderpass_info.pClearValues = &clear_values[0];

  vkCmdBeginRenderPass(m_vkrenderer.m_command_buffers[index], &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

  /*
  mesh_data_t* last_mesh = NULL;
  VkDeviceSize offsets[] = {0};
  for(size_t i = 0; (i < num_meshes) && (i < MAX_PRIMITIVE_COUNT); ++i) 
  {
    const render_header_t* header = &scene->m_headers[i];
    if(header->m_active &&
       header->m_frustrum_visible &&
       //header->m_mobility_type == TnaRenderMobilityType::E_STATIC &&
       scene->m_meshes[header->m_offset] != NULL)
    {
      mesh_data_t* mesh_data = scene->m_meshes[header->m_offset];


      if(last_mesh != mesh_data)
      {
        VkVertexBuffer* vkvertex_buffer = (VkVertexBuffer*)*mesh_data->m_vertex_buffer;
        //VkBuffer vertex_buffers[] = {vkvertex_buffer->m_buffer};

        vkCmdBindVertexBuffers(m_vkrenderer.m_command_buffers[index], 
                               0, 
                               1, 
                               &vkvertex_buffer->m_buffer, 
                               offsets);

        VkIndexBuffer* vkindex_buffer = (VkIndexBuffer*)*mesh_data->m_index_buffer;
        vkCmdBindIndexBuffer(m_vkrenderer.m_command_buffers[index], 
                             vkindex_buffer->m_buffer, 
                             0, 
                             VK_INDEX_TYPE_UINT32);
        last_mesh = mesh_data;
      }

      uint32_t offset = header->m_offset*dynamic_alignment;
      vkCmdBindDescriptorSets(m_vkrenderer.m_command_buffers[index], 
                              VK_PIPELINE_BIND_POINT_GRAPHICS, 
                              m_vkrenderer.m_pipeline_layout, 
                              0, 
                              1, 
                              &m_vkrenderer.m_descriptor_sets[index], 
                              1, 
                              &offset);

      vkCmdDrawIndexed(m_vkrenderer.m_command_buffers[index], 
                       mesh_data->m_num_indices, 
                       1, 
                       0, 
                       0, 
                       0);
    }
  }
  */

  // ADD IMGUI COMMAND BUFFER
  //gsvk_gui_build_commands(index);

  vkCmdEndRenderPass(m_vkrenderer.m_command_buffers[index]);

  if (vkEndCommandBuffer(m_vkrenderer.m_command_buffers[index]) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, 
                  "failed to record command buffer!");
  }
  return;
}

VkFormat
gsvk_find_supported_format(VkPhysicalDevice physical_device, 
                           VkFormat* candidates, 
                           uint32_t num_candidates,
                           VkImageTiling tiling,
                           VkFormatFeatureFlags feature_flags)
{
  for (uint32_t i = 0; i < num_candidates; ++i) 
  {
    VkFormat format = candidates[i];
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & feature_flags) == feature_flags) 
    {
      return format;
    } 
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & feature_flags) == feature_flags) 
    {
      return format;
    }
  }
  return VK_FORMAT_UNDEFINED;
}



VkCommandBuffer 
gsvk_begin_single_time_commands(VkDevice device, 
                           VkCommandPool command_pool) 
{
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = command_pool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device, &allocInfo, &command_buffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &beginInfo);

  return command_buffer;
}

void 
gsvk_end_single_time_commands(VkDevice device,
                         VkQueue graphics_queue,
                         VkCommandPool command_pool,
                         VkCommandBuffer command_buffer) 
{
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

uint32_t
gsvk_find_memory_type(VkMemoryRequirements* memreqs, 
                      VkMemoryPropertyFlags memprops)
{
  for (uint32_t i = 0; i < m_vkrenderer.m_pd_mem_properties.memoryTypeCount; i++) 
  {

    if ((memreqs->memoryTypeBits & (1 << i)) && 
        (m_vkrenderer.m_pd_mem_properties.memoryTypes[i].propertyFlags & memprops) 
        == memprops) {
      return i;
    }
  }
    GS_LOG_ERROR(E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
                 "Unable to find suitable memory type");
  return -1;
}




////////////////////////////////////////////////
////////////// PUBLIC METHODS //////////////////
////////////////////////////////////////////////

void 
gs_renderer_init(const gs_config_t* config, 
                 GLFWwindow* window) 
{
  gsvk_renderer_init(config, 
                     window);
}


void 
gs_renderer_release(void) 
{
  gsvk_renderer_release();
}


void
gs_renderer_begin_frame(void) 
{
  //gsvk_gui_begin_frame();
  return;
}

void 
gs_renderer_end_frame(void) 
{
	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(m_vkrenderer.m_logical_device, 
                                          m_vkrenderer.m_swap_chain, 
                                          ULONG_MAX, 
                                          m_vkrenderer.m_image_available_semaphore, 
                                          VK_NULL_HANDLE, 
                                          &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) 
  {
    gsvk_recreate_swap_chain();
    //gsvk_gui_recreate();
    return;
  } 
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, 
                  "failed to acquire swap chain image!");
  }

  gsvk_build_command_buffer(image_index, 
                            NULL);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = {m_vkrenderer.m_image_available_semaphore};
	VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &m_vkrenderer.m_command_buffers[image_index];

  VkSemaphore signal_semaphores[] = {m_vkrenderer.m_render_finished_semaphore};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  if (vkQueueSubmit(m_vkrenderer.m_graphics_queue, 
                    1, 
                    &submit_info, 
                    VK_NULL_HANDLE) != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, 
                  "failed to submit draw command buffer!");
  }

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapChains[] = {m_vkrenderer.m_swap_chain};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;
	present_info.pImageIndices = &image_index;
	present_info.pResults = NULL; // Optional

	result = vkQueuePresentKHR(m_vkrenderer.m_present_queue, 
                             &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
  {
    gsvk_recreate_swap_chain();
    //gsvk_gui_recreate();
  } 
  else if (result != VK_SUCCESS) 
  {
    GS_LOG_ERROR(E_RENDERER_RUNTIME_ERROR, 
                 "failed to present swap chain image!");
  }

  vkQueueWaitIdle(m_vkrenderer.m_present_queue);
  return;
}

void
gs_gui_init()
{
  //gsvk_gui_init();
}

void
gs_gui_release()
{
  //gsvk_gui_release();
}

gs_device_properties_t
get_device_properties()
{
  return (gs_device_properties_t){m_vkrenderer.m_pd_properties.limits.minUniformBufferOffsetAlignment};
}
