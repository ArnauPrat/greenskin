

#include "tasking_widget.h"
#include "../imgui.h"
#include "../../trace/trace.h"
#include "../../tasking/tasking.h"
#include "../../engine.h"

#include <stdio.h>
#include <limits.h>

namespace tna
{

static bool m_button_start = true;
static uint64_t  m_last_window_content_width = 512;
static const uint32_t m_min_window_content_width = 512;
static float PX_PER_MS = 100;
static double NS_PER_MS = 1000000.0;
static char m_display_info[_TNA_TRACE_MAX_INFO_STRING_LENGTH];
static double m_display_exec_time_ms = 0;

void
tasking_widget_init()
{
}

void
tasking_widget_release()
{
}



static double
ns_to_ms(uint64_t time)
{
  return (time/NS_PER_MS);
}

static double
ns_to_px(uint64_t time)
{
  return ns_to_ms(time)*PX_PER_MS;
}

void
tasking_widget_render()
{
  ImGui::SetNextWindowContentWidth(m_last_window_content_width);
  m_last_window_content_width = m_min_window_content_width;
  ImGui::Begin("Tasks", NULL, ImGuiWindowFlags_HorizontalScrollbar);                         
  ImVec2 pos = ImGui::GetWindowPos();
  ImVec2 size = ImGui::GetWindowSize();
  aabb2_t window_aabb = {};
  window_aabb.m_min = {pos.x, pos.y};
  window_aabb.m_max = {pos.x+size.x, pos.y + size.y};
  if(m_button_start)
  {
    if(ImGui::Button("Start Recording"))
    {
      m_button_start = false;
      TRACE_FLUSH();
      TRACE_ENABLE();
    }

    if(ImGui::Button("Clear"))
    {
      TRACE_FLUSH();
    }
  }
  else
  {
    if(ImGui::Button("Stop Recording"))
    {
      m_button_start = true;
      TRACE_DISABLE();
      PX_PER_MS = 100;
      m_display_info[0] = '\0';
      m_display_exec_time_ms = 0;
    }

  }


  // Add left padding
  pos.x += 20;
  // Add Start button space
  pos.y += 50;
  // Add Clear button space
  pos.y += 50;


  if(m_button_start)
  {
    // Reading input
    ImGuiIO& io = ImGui::GetIO();

    bool mouse_clicked_left = false;
    ImVec2 mouse_clicked_pos;

    if(is_edit_mode() && ImGui::IsWindowFocused())
    {
      mouse_clicked_left = io.MouseDown[0];
      mouse_clicked_pos = io.MousePos;

      if(io.MouseWheel != 0.0)
      {
        if(io.KeyCtrl)
        {
          PX_PER_MS += PX_PER_MS*(10*io.MouseWheel) / 100.0;
        } 
        else
        {
          ImGui::SetScrollX(ImGui::GetScrollX() - io.MouseWheel*50.0);
        }
      } 
    }

    uint64_t time_min = ULLONG_MAX;
    //uint64_t time_max = 0;
    for(uint32_t i = 0; i < tasking_get_num_threads(); ++i)
    {
      uint32_t count;
      trace_event_t* events = trace_get_trace_event_array(i,&count);
      for (uint32_t j = 0; j < count; ++j) 
      {
        if(events[j].m_time_ns < time_min )
        {
          time_min = events[j].m_time_ns;
        }
      }
    }

    //const ImGuiStyle& style = ImGui::GetStyle();;
    pos.x -= ImGui::GetScrollX();


    // Drawing Worker tasks 
    for(uint32_t i = 0; i <= tasking_get_num_threads(); ++i)
    {
      char text[256];
      if(i < tasking_get_num_threads())
      {
        sprintf(text, "Worker %d", i);
      }
      else
      {
        sprintf(text, "Main Thread");
      }
      ImGui::GetWindowDrawList()->AddText(pos, 
                                          ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)),
                                          text);
      pos.y += 25;
      uint64_t right_most = 0;
      uint32_t count = 0;
      trace_event_t* events = trace_get_trace_event_array(i,&count);
      uint64_t left = 0;
      int32_t mouse_selected_task = -1;
      trace_event_type_t previous_open = trace_event_type_t::E_TASK_START;
      // Drawing task rectangles
      for (uint32_t j = 0; j < count; ++j) 
      {
        if(events[j].m_event_type == trace_event_type_t::E_TASK_START || 
           events[j].m_event_type == trace_event_type_t::E_TASK_RESUME)
        {
          left = events[j].m_time_ns - time_min;
          previous_open = events[j].m_event_type;
        }
        else if(events[j].m_event_type != trace_event_type_t::E_NEW_FRAME)
        {
          if(left != 0) // there must be a TASK START OR TASK RESUME FIRST event first
          {
            uint64_t right = events[j].m_time_ns - time_min;
            ImVec2 min(pos.x + ns_to_px(left), pos.y); 
            ImVec2 max(pos.x + ns_to_px(right), min.y + 25);
            aabb2_t task_aabb = {};
            task_aabb.m_min = {min.x, min.y}; 
            task_aabb.m_max = {max.x, max.y};
            if(aabb2_overlap(&window_aabb, &task_aabb))
            {

              ImU32 color = (previous_open == trace_event_type_t::E_TASK_START) && (events[j].m_event_type == trace_event_type_t::E_TASK_STOP) ? 
              ImGui::ColorConvertFloat4ToU32(ImVec4(1, .15, .15, 1)) : 
              ImGui::ColorConvertFloat4ToU32(ImVec4(.15, 1.0, .15, 1));
              ImGui::GetWindowDrawList()->AddRectFilled(min, 
                                                        max, 
                                                        color);
              ImGui::GetWindowDrawList()->AddRect(min, 
                                                  max, 
                                                  ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)));
            }

            if(right > right_most)
            {
              right_most = right;
            }

            if(mouse_clicked_pos.x > min.x && 
               mouse_clicked_pos.x < max.x &&
               mouse_clicked_pos.y > min.y &&
               mouse_clicked_pos.y < max.y &&
               mouse_clicked_left)
            {
              mouse_selected_task = (int32_t)j;
              m_display_exec_time_ms = ns_to_ms(right - left);
            }
          }
        }
      }


      if(mouse_selected_task >=0 && mouse_clicked_left)
      {
        strncpy(m_display_info, events[mouse_selected_task].m_info, _TNA_TASKING_MAX_INFO_STRING_LENGTH);
      }

      // Drawing task text
      for (uint32_t j = 0; j < count; ++j) 
      {
        if(events[j].m_event_type == trace_event_type_t::E_TASK_START || 
           events[j].m_event_type == trace_event_type_t::E_TASK_RESUME)
        {
          left = events[j].m_time_ns - time_min;
        }
        else if(events[j].m_event_type != trace_event_type_t::E_NEW_FRAME)
        {
          if(left != 0) // there must be a TASK START OR TASK RESUME FIRST event first
          {
            double right = events[j].m_time_ns - time_min;
            ImVec2 min(pos.x + ns_to_px(left), pos.y); 
            ImVec2 max(pos.x + ns_to_px(right), min.y + 25);

            aabb2_t task_aabb = {};
            task_aabb.m_min = {min.x, min.y}; 
            task_aabb.m_max = {max.x, max.y};
            if(aabb2_overlap(&window_aabb, &task_aabb))
            {

              ImVec2 text_pos((max.x-min.x)/2 + min.x - 10, (max.y-min.y)/2 + min.y - 10);
              ImGui::GetWindowDrawList()->AddText(text_pos, 
                                                  ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)),
                                                  events[j].m_name);
            }
          }
        }
      }

      m_last_window_content_width = ns_to_px(right_most);
      // space taken by rectangles 
      pos.y += 25;
      // padding between lines
      pos.y += 10;
    }

    // add some padding between worker tasks and axis
    pos.y+=10;

    // Drawing frame times axes
    uint32_t count;
    trace_event_t* events = trace_get_trace_event_array(tasking_get_num_threads(),&count);
    for(uint32_t i = 0; i < count; ++i)
    {
      if(events[i].m_event_type == trace_event_type_t::E_NEW_FRAME)
      {
        double xpos = ((events[i].m_time_ns - time_min)/NS_PER_MS)*PX_PER_MS;
        if(xpos >= window_aabb.m_min.x && xpos <= window_aabb.m_max.x)
        {
          ImVec2 start(pos.x + xpos, ImGui::GetWindowPos().y+50);
          ImVec2 end(pos.x + xpos, pos.y);
          ImGui::GetWindowDrawList()->AddLine(start, 
                                              end, 
                                              ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1)), 
                                              2.0);
        }
      }
    }

    // Drawing X axis
    ImVec2 start(pos.x, pos.y);
    ImVec2 end(pos.x + m_last_window_content_width, pos.y);
    ImGui::GetWindowDrawList()->AddLine(start, 
                                        end, 
                                        ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1)), 
                                        2.0);
    for(uint32_t i = 0; i < m_last_window_content_width/PX_PER_MS; ++i)
    {
      ImVec2 start(pos.x + i*PX_PER_MS, pos.y-5);
      ImVec2 end(pos.x + i*PX_PER_MS, pos.y+5);

      if(start.x >= window_aabb.m_min.x && start.x <= window_aabb.m_max.x)
      {
        ImGui::GetWindowDrawList()->AddLine(start, 
                                            end, 
                                            ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1)), 
                                            2.0);


        if(i%2==0)
        {
          char text[256];
          sprintf(text, "%u ms", i);
          ImVec2 text_pos(end.x-10, end.y+5);
          ImGui::GetWindowDrawList()->AddText(text_pos, 
                                              ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)),
                                              text);
        }
      }
    }

    pos.y += 50;

    // Rendering task information
    ImVec2 info_text_pos = pos;
    info_text_pos.x += ImGui::GetScrollX(); // we correct the position not to be affected by the scroll
    if(strlen(m_display_info) > 0 )
    {
      char text[128];
      sprintf(text, "Execution time: %lf ms", m_display_exec_time_ms);
      ImGui::GetWindowDrawList()->AddText(info_text_pos, 
                                          ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)),
                                          text);

      info_text_pos.y += 10;
      ImGui::GetWindowDrawList()->AddText(info_text_pos, 
                                          ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)),
                                          m_display_info);
    }

  }

  ImGui::End();                         
}

} /* tna */ 
