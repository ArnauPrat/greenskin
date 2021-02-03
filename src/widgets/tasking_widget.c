

#include "platform.h"
#include "tasking_widget.h"
#include "../game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../third_party/cimgui/cimgui.h"
#include "../trace.h"
#include "../tasking.h"
#include "../aabb.h"
#include "../geom_tests.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>

static bool m_button_start = true;
static uint64_t  m_last_window_content_width = 512;
static const uint32_t m_min_window_content_width = 512;
static float PX_PER_MS = 100;
static double US_PER_MS = 1000.0;
static char m_display_info[GS_TRACE_MAX_INFO_LEN];
static double m_display_exec_time_ms = 0;
static ImVec2 m_record_button_size = {125,20};
static ImVec2 m_clear_button_size = {100,20};
static float m_ruler_width = 30;
static float m_worker_id_indent = 100;
static uint32_t m_legend_sqr_side = 30;

static ImVec4 colors[E_TASK_CAT_MAX_NUM] = 
{
  (ImVec4){1.0, 0.2, 1.0, 1.0},  // Pink - Unknown
  (ImVec4){1.0, 0.2, 0.2, 1.0},  // Red - Rendering 
  (ImVec4){0.2, 0.2, 1.0, 1.0},  // Blue - Resources 
  (ImVec4){0.2, 1.0, 0.2, 1.0},  // Green - System 
};

static const char* legend[E_TASK_CAT_MAX_NUM] = 
{
  "Unknown", 
  "Rendering", 
  "Resources", 
  "System"
};

void
gs_tasking_widget_init()
{
}

void
gs_tasking_widget_release()
{
}

static double
us_to_ms(uint64_t time)
{
  return (time/US_PER_MS);
}


static double
us_to_px(uint64_t time)
{
  return us_to_ms(time)*PX_PER_MS;
}

static void
draw_grid(uint32_t width, 
          uint32_t ms_offset)
{

  ImVec2 winpos;
  igGetWindowPos(&winpos);
  uint32_t start_x = igGetCursorPosX();
  uint32_t start_y = igGetCursorPosY();

  // Drawing ruler frame
  float ruler_length = width;
  ImVec2 frame_min = {winpos.x + start_x + m_worker_id_indent, winpos.y + start_y};
  ImVec2 frame_max = {frame_min.x + ruler_length, frame_min.y + m_ruler_width};
  igRenderFrame((ImVec2){frame_min.x, frame_min.y}, 
                (ImVec2){frame_max.x, frame_max.y}, 
                igColorConvertFloat4ToU32((ImVec4){0.5, 0.5, 0.5, 1}), 
                true, 
                1.0);

  igSetCursorPos((ImVec2){start_x, start_y+m_ruler_width});

  uint32_t colors[2] = {igColorConvertFloat4ToU32((ImVec4){0.3, 0.3, 0.3, 1}), 
    igColorConvertFloat4ToU32((ImVec4){0.4, 0.4, 0.5, 1})};
  for(uint32_t i = 0; i <= gs_tasking_get_num_threads(); ++i)
  {
    ImVec2 frame_min = {winpos.x + start_x + m_worker_id_indent, winpos.y + start_y + m_ruler_width*(i+1)};
    ImVec2 frame_max = {frame_min.x + ruler_length, frame_min.y + m_ruler_width};
    igRenderFrame((ImVec2){frame_min.x, frame_min.y}, 
                  (ImVec2){frame_max.x, frame_max.y}, 
                  colors[i%2], 
                  true, 
                  1.0);
  }

  // Drawing workers and main thread frames 
  for(uint32_t i = 0; i <= gs_tasking_get_num_threads(); ++i)
  {
    igSetCursorPos((ImVec2){start_x, start_y+i*m_ruler_width+m_ruler_width + m_ruler_width/3});
    char text[256];
    if(i < gs_tasking_get_num_threads())
    {
      sprintf(text, "Worker %d", i);
    }
    else
    {
      sprintf(text, "Main Thread");
    }
    // Draw background grid
    igText(text);
  }
  igSetCursorPos((ImVec2){start_x, start_y+(gs_tasking_get_num_threads()+1)*m_ruler_width+m_ruler_width});
  igSeparator();

  // Drawing ms lines
  ImDrawList* draw_list = igGetWindowDrawList();
  uint32_t max_ticks = width / PX_PER_MS;
  for(uint32_t i = 0; i < max_ticks; i++)
  {

    ImDrawList_AddLine(draw_list, 
                       (ImVec2){frame_min.x + i*PX_PER_MS, frame_min.y + 10}, 
                       (ImVec2){frame_min.x + i*PX_PER_MS, frame_min.y + (m_ruler_width*(gs_tasking_get_num_threads()+1+1))},
                       igColorConvertFloat4ToU32((ImVec4){0.8, 0.8, 0.8, 1}), 
                       1.0);

    char text[256];
    sprintf(text, "%u ms", i );
    ImVec2 text_pos = {frame_min.x+i*PX_PER_MS+5, frame_min.y + m_ruler_width - 15};

    ImDrawList_AddTextVec2(draw_list, 
                           text_pos, 
                           igColorConvertFloat4ToU32((ImVec4){0, 0, 0, 1}),
                           text, 
                           text + strlen(text));

  }
}

void
gs_tasking_widget_render()
{
  //igSetNextWindowContentWidth(m_last_window_content_width);
  m_last_window_content_width = m_min_window_content_width;

  igBegin("Tasks", NULL, ImGuiWindowFlags_AlwaysHorizontalScrollbar);                         

  ImVec2 window_pos;
  igGetWindowPos(&window_pos);
  ImVec2 window_size;
  igGetWindowSize(&window_size);
  gs_aabb2_t window_aabb = {};
  window_aabb.m_min = (gs_vec2float_t){window_pos.x, window_pos.y};
  window_aabb.m_max = (gs_vec2float_t){window_pos.x+window_size.x, window_pos.y + window_size.y};

  if(m_button_start)
  {
    if(igButton("Start Recording", m_record_button_size))
    {
      m_button_start = false;
      GS_TRACE_FLUSH();
      GS_TRACE_ENABLE();
    }

    igSameLine(m_record_button_size.x, 10);

    if(igButton("Clear", m_clear_button_size))
    {
      GS_TRACE_FLUSH();
    }
  }
  else
  {
    if(igButton("Stop Recording", m_record_button_size))
    {
      m_button_start = true;
      GS_TRACE_DISABLE();
      PX_PER_MS = 100;
      m_display_info[0] = '\0';
      m_display_exec_time_ms = 0;
    }
  }
  igSeparator();

  double min_us = INT_MAX;
  double max_us = 0;
  bool found = false;
  if(m_button_start)
  {
    // Reading input
    ImGuiIO io = *igGetIO();

    //bool mouse_clicked_left = false;

    if(gs_is_edit_mode() && igIsWindowFocused(ImGuiWindowFlags_None))
    {
      //ImVec2 mouse_clicked_pos;
      //mouse_clicked_left = io.MouseDown[0];
      //mouse_clicked_pos = io.MousePos;

      if(io.MouseWheel != 0.0)
      {
        if(io.KeyCtrl)
        {
          PX_PER_MS += PX_PER_MS*(10*io.MouseWheel) / 100.0;
        } 
        else
        {
          igSetScrollXFloat(igGetScrollX() - io.MouseWheel*50.0);
        }
      } 
    }

    for(uint32_t i = 0; i <= gs_tasking_get_num_threads(); ++i)
    {
      uint32_t count = 0;
      gs_trace_event_t* events = gs_trace_get_trace_event_array(i,&count);
      if(count > 0)
      {
        found = true;
        double us = events[0].m_time_us;
        if(us < min_us)
        {
          min_us = us;
        }

        us = events[count-1].m_time_us;
        if(us > max_us)
        {
          max_us = us;
        }
      }
    }
  }

  // Reading input
  ImGuiIO io = *igGetIO();
  uint32_t grid_posx = igGetCursorPosX();
  uint32_t grid_posy = igGetCursorPosY();
  double span = ((max_us - min_us)/US_PER_MS)/PX_PER_MS;
  uint32_t grid_width = (found && span > window_size.x) ? span : window_size.x;
  draw_grid(grid_width, 0);

  uint32_t legend_posx = igGetCursorPosX();
  uint32_t legend_posy = igGetCursorPosY();

  for(uint32_t i = 0; i < E_TASK_CAT_MAX_NUM; ++i)
  {
    ImVec2 frame_min =  (ImVec2){window_pos.x + legend_posx + m_legend_sqr_side*i+i*100, window_pos.y + legend_posy}; 
    ImVec2 frame_max = (ImVec2){frame_min.x + m_legend_sqr_side, frame_min.y + m_legend_sqr_side};
    ImU32 color = igColorConvertFloat4ToU32(colors[i]);
    igRenderFrame(frame_min, 
                  frame_max, 
                  color, 
                  true, 
                  1.0);

    igRenderText((ImVec2){frame_max.x + 20, (frame_max.y - frame_min.y)/3 + frame_min.y}, 
                 legend[i], 
                 legend[i] + strlen(legend[i]), 
                 false);
  }



  if(m_button_start)
  {
    for(uint32_t i = 0; i <= gs_tasking_get_num_threads(); ++i)
    {
      uint32_t count = 0;
      gs_trace_event_t* events = gs_trace_get_trace_event_array(i,&count);
      uint64_t left = 0;
      for (uint32_t j = 0; j < count; ++j) 
      {
        if(events[j].m_event_type == E_TRACE_TASK_START || 
           events[j].m_event_type == E_TRACE_TASK_RESUME)
        {
          left = events[j].m_time_us - min_us;
        }
        else if(events[j].m_event_type != E_TRACE_NEW_FRAME)
        {
          if(left != 0) // there must be a TASK START OR TASK RESUME FIRST event first
          {
            uint64_t right = events[j].m_time_us - min_us;
            ImVec2 frame_min =  (ImVec2){window_pos.x + grid_posx + us_to_px(left) + m_worker_id_indent, window_pos.y + grid_posy + (i+1)*m_ruler_width}; 
            ImVec2 frame_max = (ImVec2){window_pos.x + grid_posx + us_to_px(right) + m_worker_id_indent, frame_min.y + m_ruler_width};
            gs_aabb2_t task_aabb = {};
            task_aabb.m_min = (gs_vec2float_t){frame_min.x, frame_min.y}; 
            task_aabb.m_max = (gs_vec2float_t){frame_max.x, frame_max.y};
            if(gs_aabb2_aabb2_overlap(&window_aabb, &task_aabb))
            {

              ImU32 color = igColorConvertFloat4ToU32(colors[events[j].m_task_category]);
              igRenderFrame(frame_min, 
                            frame_max, 
                            color, 
                            true, 
                            1.0);
              if(gs_is_edit_mode())
              {
                gs_vec2float_t tmp_mouse_pos = {io.MousePos.x, io.MousePos.y};
                gs_aabb2_t tmp_box = {{frame_min.x, frame_min.y}, {frame_max.x, frame_max.y}};
                if(gs_aabb2_point2_overlap(&tmp_box, &tmp_mouse_pos))
                {
                  igBeginTooltip();
                  igPushTextWrapPos(igGetFontSize() * 35.0f);
                  igTextUnformatted(events[j].m_info, events[j].m_info + strlen(events[j].m_info));
                  igPopTextWrapPos();
                  igEndTooltip();
                }
              }
            }
          }
        }
      }
    }

  }



  /*


  // Drawing Worker tasks 
  for(uint32_t i = 0; i <= gs_tasking_get_num_threads(); ++i)
  {
  char text[256];
  if(i < gs_tasking_get_num_threads())
  {
  sprintf(text, "Worker %d", i);
  }
  else
  {
  sprintf(text, "Main Thread");
  }
  ImDrawList* draw_list = igGetWindowDrawList();
  ImDrawList_AddTextVec2(draw_list, pos,igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}),
  text, text + strlen(text));
  pos.y += 25;
  uint64_t right_most = 0;
  uint32_t count = 0;
  gs_trace_event_t* events = gs_trace_get_trace_event_array(i,&count);
  uint64_t left = 0;
  int32_t mouse_selected_task = -1;
  gs_trace_event_type_t previous_open = E_TRACE_TASK_START;
  // Drawing task rectangles
  for (uint32_t j = 0; j < count; ++j) 
  {
  if(events[j].m_event_type == E_TRACE_TASK_START || 
  events[j].m_event_type == E_TRACE_TASK_RESUME)
  {
  left = events[j].m_time_us - time_min;
  previous_open = events[j].m_event_type;
  }
  else if(events[j].m_event_type != E_TRACE_NEW_FRAME)
  {
  if(left != 0) // there must be a TASK START OR TASK RESUME FIRST event first
  {
  uint64_t right = events[j].m_time_us - time_min;
  ImVec2 min =  (ImVec2){pos.x + ns_to_px(left), pos.y}; 
  ImVec2 max = (ImVec2){pos.x + ns_to_px(right), min.y + 25};
  gs_aabb2_t task_aabb = {};
  task_aabb.m_min = (gs_vec2float_t){min.x, min.y}; 
  task_aabb.m_max = (gs_vec2float_t){max.x, max.y};
  if(gs_aabb2_overlap(&window_aabb, &task_aabb))
  {

  ImU32 color = (previous_open == E_TRACE_TASK_START) && (events[j].m_event_type == E_TRACE_TASK_STOP) ? 
  igColorConvertFloat4ToU32((ImVec4){1, .15, .15, 1}) : 
  igColorConvertFloat4ToU32((ImVec4){.15, 1.0, .15, 1});
  ImDrawList_AddRectFilled(draw_list, 
  min, 
  max, 
  color, 
  1.0, 
  ImDrawCornerFlags_None);

  ImDrawList_AddRect(draw_list, 
  min, 
  max, 
  igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}), 
  1.0, 
  ImDrawCornerFlags_None, 
  1.0);
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
  strncpy(m_display_info, events[mouse_selected_task].m_info, GS_TASKING_MAX_INFO_LEN);
}

// Drawing task text
for (uint32_t j = 0; j < count; ++j) 
{
  if(events[j].m_event_type == E_TRACE_TASK_START || 
     events[j].m_event_type == E_TRACE_TASK_RESUME)
  {
    left = events[j].m_time_us - time_min;
  }
  else if(events[j].m_event_type != E_TRACE_NEW_FRAME)
  {
    if(left != 0) // there must be a TASK START OR TASK RESUME FIRST event first
    {
      double right = events[j].m_time_us - time_min;
      ImVec2 min = (ImVec2){pos.x + ns_to_px(left), pos.y}; 
      ImVec2 max = (ImVec2){pos.x + ns_to_px(right), min.y + 25};

      gs_aabb2_t task_aabb = {};
      task_aabb.m_min = (gs_vec2float_t){min.x, min.y}; 
      task_aabb.m_max = (gs_vec2float_t){max.x, max.y};
      if(gs_aabb2_overlap(&window_aabb, &task_aabb))
      {

        ImVec2 text_pos = (ImVec2){(max.x-min.x)/2 + min.x - 10, (max.y-min.y)/2 + min.y - 10};
        ImDrawList_AddTextVec2(draw_list, 
                               text_pos, 
                               igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}),
                               events[j].m_name, 
                               events[j].m_name + strlen(events[j].m_name));
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
gs_trace_event_t* events = gs_trace_get_trace_event_array(gs_tasking_get_num_threads(),&count);
for(uint32_t i = 0; i < count; ++i)
{
  if(events[i].m_event_type == E_TRACE_NEW_FRAME)
  {
    double xpos = ((events[i].m_time_us - time_min)/US_PER_MS)*PX_PER_MS;
    if(xpos >= window_aabb.m_min.m_x && xpos <= window_aabb.m_max.m_x)
    {
      ImVec2 window_pos;
      igGetWindowPos(&window_pos);
      ImVec2 start = (ImVec2){pos.x + xpos, window_pos.y+50};
      ImVec2 end = (ImVec2){pos.x + xpos, pos.y};

      ImDrawList* draw_list = igGetWindowDrawList();
      ImDrawList_AddLine(draw_list, 
                         start, 
                         end, 
                         igColorConvertFloat4ToU32((ImVec4){0, 1, 0, 1}), 
                         2.0);
    }
  }
}

// Drawing X axis
ImVec2 start = {pos.x, pos.y};
ImVec2 end = {pos.x + m_last_window_content_width, pos.y};
ImDrawList* draw_list = igGetWindowDrawList();
ImDrawList_AddLine(draw_list, 
                   start, 
                   end, 
                   igColorConvertFloat4ToU32((ImVec4){0, 1, 0, 1}), 
                   2.0);
for(uint32_t i = 0; i < m_last_window_content_width/PX_PER_MS; ++i)
{
  ImVec2 start = {pos.x + i*PX_PER_MS, pos.y-5};
  ImVec2 end = {pos.x + i*PX_PER_MS, pos.y+5};

  if(start.x >= window_aabb.m_min.m_x && start.x <= window_aabb.m_max.m_x)
  {
    ImDrawList* draw_list = igGetWindowDrawList();
    ImDrawList_AddLine(draw_list, 
                       start, 
                       end, 
                       igColorConvertFloat4ToU32((ImVec4){0, 1, 0, 1}), 
                       2.0);


    if(i%2==0)
    {
      char text[256];
      sprintf(text, "%u ms", i);
      ImVec2 text_pos = {end.x-10, end.y+5};

      ImDrawList_AddTextVec2(draw_list, 
                             text_pos, 
                             igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}),
                             text, 
                             text + strlen(text));
    }
  }
}

pos.y += 50;

// Rendering task information
ImVec2 info_text_pos = pos;
info_text_pos.x += igGetScrollX(); // we correct the position not to be affected by the scroll
if(strlen(m_display_info) > 0 )
{
  char text[128];
  sprintf(text, "Execution time: %lf ms", m_display_exec_time_ms);
  ImDrawList* draw_list = igGetWindowDrawList();
  ImDrawList_AddTextVec2(draw_list, 
                         info_text_pos, 
                         igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}),
                         text, 
                         text+strlen(text));

  info_text_pos.y += 10;
  ImDrawList_AddTextVec2(draw_list, 
                         info_text_pos, 
                         igColorConvertFloat4ToU32((ImVec4){1, 1, 1, 1}),
                         m_display_info, 
                         m_display_info + strlen(m_display_info));
}

}
*/

igEnd();                         
}

