



#include "../../../log.h"
#include "../../../error.h"
#include "../vkrenderer.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../../third_party/cimgui/cimgui.h"

#include "../../third_party/cimgui/imgui/impl/imgui_impl_glfw.h"
#include "../../third_party/cimgui/imgui/impl/imgui_impl_vulkan.h"

static ImGuiContext* ctx;

static void check_vk_result(VkResult err)
{
    if (err == 0) return;

    GS_LOG_ERROR(E_RENDERER_GUI_ERROR, "GUI error");
}

void
gsvk_gui_init()
{

  ctx = igCreateContext(NULL);
  ImGuiIO io= *igGetIO(); ;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  igStyleColorsDark(NULL);
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForVulkan(m_vkrenderer.p_window, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = m_vkrenderer.m_vulkan_instance;
  init_info.PhysicalDevice = m_vkrenderer.m_physical_device;
  init_info.Device = m_vkrenderer.m_logical_device;
  init_info.QueueFamily = m_vkrenderer.m_gqueue_index;
  init_info.Queue = m_vkrenderer.m_graphics_queue;
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = m_vkrenderer.m_descriptor_pool;
  init_info.Allocator = VK_NULL_HANDLE;
  init_info.MinImageCount = m_vkrenderer.m_num_swap_chain_images;
  init_info.ImageCount = m_vkrenderer.m_num_swap_chain_images;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info, m_vkrenderer.m_render_pass);

  {
    // Use any command queue
    VkCommandPool command_pool = m_vkrenderer.m_command_pool;
    VkCommandBuffer command_buffer = m_vkrenderer.m_command_buffers[0];

    VkResult res = vkResetCommandPool(m_vkrenderer.m_logical_device, command_pool, 0);
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
    res = vkQueueSubmit(m_vkrenderer.m_graphics_queue, 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(res);

    res = vkDeviceWaitIdle(m_vkrenderer.m_logical_device);
    check_vk_result(res);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

}

void
gsvk_gui_recreate()
{
  ImGui_ImplVulkan_SetMinImageCount(m_vkrenderer.m_num_swap_chain_images);
}

void
gsvk_gui_begin_frame()
{
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
}

void
gsvk_gui_build_commands(uint32_t index)
{
  ImGui_ImplVulkan_RenderDrawData(igGetDrawData(), m_vkrenderer.m_command_buffers[index]);
}

void
gsvk_gui_release()
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(ctx);
}
