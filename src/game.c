
#include "platform.h"
#include "config.h"
#include "log.h"
#include "memory.h"
#include "trace.h"
#include "tasking.h"
#include "resources.h"
#include "renderer.h"
#include "input.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "third_party/cimgui/cimgui.h"
#include "widgets/tasking_widget.h"
//#include "rendering/rendering.h"
//#include "resources/resources.h"
//#include "gui/imgui.h"
//#include "trace/trace.h"
//#include "tasking/atomic_counter.h"
//#include "tasking/barrier.h"
//#include "data/queue.h"
//#include "gui/widgets/tasking_widget.h"
//#include "furious/scheduler.h"
//
//#include "components/proj_view_matrix.h"
//#include "components/viewport.h"
//#include "components/transform.h"
//#include "components/transform_matrix.h"
//#include "components/render_mesh_data.h"
//
//#include <chrono>
//#include <condition_variable>
//#include <mutex>
//#include <glm/glm.hpp>
//#include <furious/furious.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static gs_config_t         m_config;
static GLFWwindow*         p_window  = NULL;
static bool                m_show_gui = true;
static bool                m_edit_mode = false;
static double              m_mouse_old_pos_x = 0.0;
static double              m_mouse_old_pos_y = 0.0;
static double              m_mouse_delta_pos_x = 0.0;
static double              m_mouse_delta_pos_y = 0.0;

//static fdb_database_t*     p_database = NULL;;
//static float               m_last_game_loop_time = 0.0f;
//static bool                m_ready = false;
//static furious_sched_t     m_frame_scheduler;
//static furious_sched_t     m_postframe_scheduler;
//
//static std::condition_variable*     m_main_thread_cond = NULL;
//static std::mutex*                  m_main_thread_mutex = NULL;

void gs_toggle_gui();
void gs_toggle_edit_mode();
bool gs_is_edit_mode();

void
gs_process_key_edit(struct GLFWwindow* window, 
                   int key, 
                   int scancode, 
                   int action, 
                   int mods)
{
  // General key commands
  switch(key)
  {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case GLFW_KEY_G:
      if (action == GLFW_PRESS)
      {
        gs_toggle_gui();
      }
      break;
    case GLFW_KEY_E:
      if (action == GLFW_PRESS)
      {
        gs_toggle_edit_mode();
        glfwGetCursorPos(p_window, 
                         &m_mouse_old_pos_x, 
                         &m_mouse_old_pos_y);

        m_mouse_delta_pos_x = 0;
        m_mouse_delta_pos_y = 0;
      }
  }
}

void 
gs_process_mouse_pos_edit(struct GLFWwindow* window, 
                           double xpos, 
                           double ypos)
{
}

void 
gs_process_mouse_button_edit(struct GLFWwindow* window, 
                            int button, 
                            int action, 
                            int mods)
{
}

static void
gs_init(void) 
{
  // Initializing loging system
  gs_log_init("./logfile.log");

  GS_LOG_INFO("Initializing memory");
  gs_mem_init();

  GS_LOG_INFO("Reading config");
  gs_config_init(&m_config, "./config.ini");

  GS_LOG_INFO("Initializing trace module");
  gs_trace_init(m_config.m_nthreads+1);

  GS_LOG_INFO("Initializing tasking module");
  gs_tasking_init(m_config.m_nthreads);

  GS_LOG_INFO("Initializing resources module");
  gs_resources_init();
  for(uint32_t i = 0; i < m_config.m_nrdirs; ++i)
  {
    gs_resources_add_rdir(m_config.m_rdirs[i]);
  }


  // Creating Window
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  p_window = glfwCreateWindow(m_config.m_vwidth, 
                              m_config.m_vheight, 
                              "GREENSKIN", 
                              m_config.m_fscreen ? glfwGetPrimaryMonitor() : NULL,
                              NULL);
  if (!p_window) 
  {
    GS_LOG_ERROR(E_RENDERER_INITIALIZATION_ERROR, 
                  "Error creating window");
  }

  glfwMakeContextCurrent(p_window);
  glfwSetKeyCallback(p_window, gs_key_callback);
  glfwSetMouseButtonCallback(p_window, gs_mouse_button_callback);
  glfwSetCursorPosCallback(p_window, gs_mouse_pos_callback);
  
  glfwSetInputMode(p_window, 
                   GLFW_CURSOR,
                   GLFW_CURSOR_DISABLED);

  glfwSetCursorPos(p_window, 
                   m_config.m_vwidth/2.0, 
                   m_config.m_vheight/2.0);

  GS_LOG_INFO("Initializing rendering module");
  gs_renderer_init(&m_config, p_window);
  
  GS_LOG_INFO("Initializing gui module");
  gs_gui_init();

  GS_LOG_INFO("Initializing input module");
  gs_input_init();


  gs_input_register_callbacks((gs_input_callbacks_t){gs_process_key_edit, 
                              gs_process_mouse_pos_edit, 
                              gs_process_mouse_button_edit}
                              );
  /*
  rendering_scene_init();
  */

  return;
}

static void
gs_release(void) 
{
  /*
  if(p_current_app != NULL) 
  {
    p_current_app->on_app_finish();
  }

  rendering_scene_release();
  terminate_gui();
  terminate_renderer();
  

  if(p_window != NULL) 
  {
    glfwDestroyWindow(p_window);
  }
  glfwTerminate();

  resources_release();
  config_release(&m_config);
  tasking_stop_thread_pool();
  trace_release();
  directory_registry_release();
  */

  GS_LOG_INFO("Releasing input module");
  gs_input_release();

  GS_LOG_INFO("Releasing gui module");
  gs_gui_release();

  GS_LOG_INFO("Releasing rendering module");
  gs_renderer_release();

  GS_LOG_INFO("Releasing resources module");
  gs_resources_release();

  GS_LOG_INFO("Releasing tasking module");
  gs_tasking_release();

  GS_LOG_INFO("Releasing trace module");
  gs_trace_release();

  GS_LOG_INFO("Releasing config");
  gs_config_release(&m_config);

  GS_LOG_INFO("Releaseing memory");
  gs_mem_release();

  GS_LOG_INFO("Releasing log module");
  gs_log_release();
  return;
}

void
gs_toggle_gui()
{
  m_show_gui = !m_show_gui;
}

void
gs_toggle_edit_mode()
{
  if(m_edit_mode)
  {
    glfwSetInputMode(p_window, 
                     GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED);
  }
  else
  {
    glfwSetInputMode(p_window, 
                     GLFW_CURSOR,
                     GLFW_CURSOR_NORMAL);

    //viewport_t* viewport = FURIOUS_FIND_GLOBAL(p_database, viewport_t);
    //glfwSetCursorPos(p_window, 
    //                 viewport->m_width/2.0, 
    //                 viewport->m_height/2.0);

  }
  m_edit_mode = !m_edit_mode;
}

bool
gs_is_edit_mode()
{
  return m_edit_mode;
}

void
gs_draw_gui()
{
  igNewFrame();
  if(m_show_gui)
  {
    //bool aux = true; ImGui::ShowDemoWindow(&aux);

    igBegin("TNA",NULL, ImGuiWindowFlags_None);                         
    igText("Frame time %.3f ms",
                1000.0f / igGetIO()->Framerate);

    //igText("FPS %.1f", 
    //            ImGui::GetIO().Framerate);

    //igText("Game Loop time: %.5f", 
    //            m_last_game_loop_time);
    igEnd();

    gs_tasking_widget_render();

  }
  igRender();
}
/*


void update_game_logic(void* ptr)
{

  auto game_loop_start = std::chrono::high_resolution_clock::now();
  float delta = *(float*)(ptr);
  furious_sched_run(&m_frame_scheduler, delta, p_database);
  furious_sched_run(&m_postframe_scheduler, delta, p_database);

  auto game_loop_end = std::chrono::high_resolution_clock::now();
  m_last_game_loop_time = std::chrono::duration<float, std::chrono::milliseconds::period>(game_loop_end - game_loop_start).count();

  std::unique_lock<std::mutex> lock(*m_main_thread_mutex);
  m_ready = true;
  m_main_thread_cond->notify_one();
}
*/

void
gs_run() 
{

  /*1
  p_database = new furious::Database();
  ///p_database->start_webserver("localhost", "8080");

  /// MANUALLY CREATING TABLES WITH REQUIRED MANUAL DESTRUCTORS
  FURIOUS_CREATE_TABLE_DESTRUCTOR((p_database), 
                                  render_mesh_data_t,
                                  render_mesh_data_release);

  /// ADDING GLOBALS TO FURIOUS
  FURIOUS_CREATE_GLOBAL((p_database),
                        projview_matrix_t);

  viewport_t* viewport = FURIOUS_CREATE_GLOBAL((p_database),
                                                viewport_t);
  viewport->m_width = m_config.m_viewport_width;
  viewport->m_height = m_config.m_viewport_height;

  // Initializing Furious
  furious::furious_init(p_database);

  p_current_app = game_app;
  p_current_app->m_state.p_database = p_database;
  p_current_app->m_state.p_window = p_window;

  p_current_app->on_app_start();

  static auto start_time = std::chrono::high_resolution_clock::now();
  furious::task_graph_t* task_graph = furious::furious_task_graph();
  furious_sched_init(&m_frame_scheduler, task_graph);
  furious::task_graph_t* post_task_graph = furious::furious_post_task_graph();
  furious_sched_init(&m_postframe_scheduler, post_task_graph);

  m_main_thread_mutex   = new std::mutex();
  m_main_thread_cond    = new std::condition_variable();
  */

  while (!glfwWindowShouldClose(p_window)) 
  {
    GS_TRACE_RECORD(m_config.m_nthreads, 
                    E_TRACE_NEW_FRAME, 
                    E_TASK_CAT_NONE,
                    NULL,
                    NULL);
    /*
    
    // Keep running
    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
    start_time = current_time;
    */

    glfwPollEvents();
    gs_renderer_begin_frame();
    /*
    p_current_app->on_frame_start(time);

    {
      std::unique_lock<std::mutex> lock(*m_main_thread_mutex);
      m_ready = false;
      task_t task;
      task.p_args = &time;
      task.p_fp = update_game_logic;
      tasking_execute_task_async(0, 
                                 task, 
                                 NULL,
                                 "update_game_logic", 
                                 "update_game_logic");

      m_main_thread_cond->wait(lock, 
                               [&] () {
                               return m_ready;
                               }
                              );
    }


    p_current_app->on_frame_end();

    */
    GS_TRACE_RECORD(m_config.m_nthreads, 
                    E_TRACE_TASK_START,
                    E_TASK_CAT_RENDERING, 
                    "Rendering", 
                    "Rendering");
    gs_draw_gui();
    gs_renderer_end_frame();


    GS_TRACE_RECORD(m_config.m_nthreads, 
                    E_TRACE_TASK_STOP,
                    E_TASK_CAT_RENDERING,
                    "Rendering", 
                    "Rendering");
  }

  /*
  delete m_main_thread_mutex;
  delete m_main_thread_cond;
  furious_sched_release(&m_frame_scheduler);
  furious_sched_release(&m_postframe_scheduler);
  p_current_app->on_app_finish();
  p_current_app = NULL;

  if(p_database)
  {
    furious::furious_release();
    //p_database->stop_webserver();
    delete p_database;
  }
  */
  return;
}


int
main(int argc, char** argv)
{
  gs_init();
  gs_run();
  gs_release();
  return 0;
}
