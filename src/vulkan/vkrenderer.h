


#ifndef _GS_VKRENDERER_H_
#define _GS_VKRENDERER_H_

#include "../platform.h"
#include "../renderer.h"
#include "../../memory.h"
#include "vkimage.h"
#include <vulkan/vulkan.h>

typedef struct gsvk_renderer_t 
{
  gs_stack_alloc_t                      m_stack_alloc;      ///< The stack allocator of the renderer
  gs_stack_alloc_t                      m_sc_stack_alloc;   ///< This stack allocator is used for resources that might need to be recreated and that are related to swap chains
  GLFWwindow*                           p_window;

  uint32_t                              m_viewport_width;   ///< Width of the viewport
  uint32_t                              m_viewport_height;  ///< Height of the viewport

  VkInstance                            m_vulkan_instance;  ///< Vulkan instance
  VkSurfaceKHR                          m_window_surface;   ///< Rendering surface
  VkDebugReportCallbackEXT              m_report_callback;  ///< Debug report callback
  VkPhysicalDevice                      m_physical_device;  ///< Physical device
  VkDevice                              m_logical_device ;  ///< Logical device
  VkSwapchainKHR                        m_swap_chain;       ///< Swap chain
  VkPipelineLayout                      m_pipeline_layout;  ///< pipeline layout
  VkRenderPass                          m_render_pass;      ///< Render pass object
  VkPipeline                            m_pipeline;         ///< Rendering pipeline
  VkCommandPool                         m_command_pool;     ///< Rendering command pool
  VkDescriptorPool                      m_descriptor_pool;  ///< The descriptor pool
  VkDescriptorSetLayout                 m_descriptor_set_layout; ///< The descriptor set layout
  VkQueue                               m_graphics_queue;   ///< Graphics command queue
  VkQueue                               m_present_queue;    ///< Present command queue

  // Physical Device Properties and Capabilities
  VkPhysicalDeviceProperties            m_pd_properties;    ///< Physical device properties
  VkPhysicalDeviceMemoryProperties      m_pd_mem_properties;///< Physical device memory properties
  VkSurfaceCapabilitiesKHR              m_pd_scaps;         ///< Physical device surface capabilities
  VkSurfaceFormatKHR*                   m_pd_sformats;      ///< The supported surface formats
  uint32_t                              m_num_pd_sformats;  ///< The number of supported surface formats
  VkPresentModeKHR*                     m_pd_pmodes;        ///< The supported present modes
  uint32_t                              m_num_pd_pmodes;        ///< The number of supported present modes


  VkSurfaceFormatKHR                    m_surface_format;   ///< Rendering Surface format
  VkExtent2D                            m_extent;           ///< Rendering Surface extent
  int32_t 			                        m_gqueue_index;     ///< The graphics queue family index
  int32_t 			                        m_pqueue_index;     ///< The present queue family index 

  VkFormat                              m_depth_format;
  gsvk_image_t                          m_depth_image;            ///< The depth buffer image
  VkImageView                           m_depth_image_view;       ///< The depth buffer image view

  VkImage*                              m_swap_chain_images;       ///< Vector with the different swap chain images
  VkImageView*                          m_swap_chain_image_views;  ///< Vector with the different swap chain image views
  VkFramebuffer*                        m_frame_buffers;           ///< Vector with the different frame buffers. One per swap chain image
  VkCommandBuffer*                      m_command_buffers;    ///< Vector with the different command buffers. One per swap chain image 
  VkDescriptorSet*                      m_descriptor_sets;   ///< The descriptor set of the pipeline
  const char**                          m_validation_layers;  ///< validation layers
  const char**                          m_device_extensions;  ///< Vector with the required physical device extensions

  uint32_t                              m_num_swap_chain_images;      ///< Number of swap chain images
  uint32_t                              m_num_swap_chain_image_views; ///< Number of wap chain image views
  uint32_t                              m_num_validation_layers;      ///< Number of validation layers
  uint32_t                              m_num_device_extensions;      ///< Number of required physical device extensions
  uint32_t                              m_num_frame_buffers;          ///< Number of frame buffers 
  uint32_t                              m_num_command_buffers;        ///< Number of command buffers
  uint32_t                              m_num_descriptor_sets;        ///< Number of command buffers


  VkSemaphore                           m_image_available_semaphore; ///< Semaphore for synchronizing next frame image retrieval
  VkSemaphore                           m_render_finished_semaphore; ///< Sempahore to synchronize render pass finished

} gsvk_renderer_t;

extern gsvk_renderer_t m_vkrenderer;

VkFormat
gsvk_find_supported_format(VkPhysicalDevice physical_device,
                           VkFormat* candidates, 
                           uint32_t num_candidates,
                           VkImageTiling tiling,
                           VkFormatFeatureFlags feature_flags);

VkCommandBuffer 
gsvk_begin_single_time_commands(VkDevice device, 
                           VkCommandPool command_pool);

  
void 
gsvk_end_single_time_commands(VkDevice device,
                         VkQueue graphics_queue,
                         VkCommandPool command_pool,
                         VkCommandBuffer command_buffer);

uint32_t
gsvk_find_memory_type(VkMemoryRequirements* memreqs, 
                      VkMemoryPropertyFlags memprops);

#endif 
