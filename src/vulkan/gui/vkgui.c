



#include "../../../log.h"
#include "../../../error.h"
#include "../vkrenderer.h"
#include "imgui_impl_vulkan.h"
#include "../../../gui/imgui_impl_glfw.h"

static void check_vk_result(VkResult err)
{
    if (err == 0) return;

    GS_LOG_ERROR(E_RENDERER_GUI_ERROR, "GUI error");
}

void
gsvk_gui_init()
{

  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForVulkan(p_renderer->p_window, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = p_renderer->m_vulkan_instance;
  init_info.PhysicalDevice = p_renderer->m_physical_device;
  init_info.Device = p_renderer->m_logical_device;
  init_info.QueueFamily = p_renderer->m_queue_indices.m_graphics_queue;
  init_info.Queue = p_renderer->m_graphics_queue;
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = p_renderer->m_descriptor_pool;
  init_info.Allocator = VK_NULL_HANDLE;
  init_info.MinImageCount = p_renderer->m_num_swap_chain_images;
  init_info.ImageCount = p_renderer->m_num_swap_chain_images;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info, p_renderer->m_render_pass);

  {
    // Use any command queue
    VkCommandPool command_pool = p_renderer->m_command_pool;
    VkCommandBuffer command_buffer = p_renderer->m_command_buffers[0];

    VkResult res = vkResetCommandPool(p_renderer->m_logical_device, command_pool, 0);
    check_vk_result(res);
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    res = vkBeginCommandBuffer(command_buffer, &begin_info);
    check_vk_result(res);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &command_buffer;
    res = vkEndCommandBuffer(command_buffer);
    check_vk_result(res);
    res = vkQueueSubmit(p_renderer->m_graphics_queue, 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(res);

    res = vkDeviceWaitIdle(p_renderer->m_logical_device);
    check_vk_result(res);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

}

void
vkgui_recreate()
{
  ImGui_ImplVulkan_SetMinImageCount(p_renderer->m_num_swap_chain_images);
}

void
vkgui_begin_frame()
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
}

void
vkgui_build_commands(uint32_t index)
{
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), p_renderer->m_command_buffers[index]);
}

void
vkgui_terminate()
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
