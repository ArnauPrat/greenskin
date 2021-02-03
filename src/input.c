
#include "input.h"
#include "log.h"

#include <string.h>

static
gs_input_callbacks_t m_input_callbacks[GS_INPUT_MAX_CALLBACKS];
uint32_t m_num_callbacks;


void
gs_input_init()
{
  memset(m_input_callbacks, 0, sizeof(gs_input_callbacks_t)*GS_INPUT_MAX_CALLBACKS);
  m_num_callbacks = 0;
}

void
gs_input_release()
{
}

uint32_t 
gs_input_register_callbacks(gs_input_callbacks_t callbacks)
{
  GS_ASSERT(m_num_callbacks < GS_INPUT_MAX_CALLBACKS && "Number of registered input callbacks exceeded the maximum");
  uint32_t cid = m_num_callbacks++;
  m_input_callbacks[cid] = callbacks;
  return cid;
}

void 
gs_input_unregister_callbacks(uint32_t cid)
{
  memcpy(&m_input_callbacks[cid], 
         &m_input_callbacks[cid+1], 
         sizeof(gs_input_callbacks_t)*(m_num_callbacks - (cid+1)));
  m_num_callbacks--;
}

void 
gs_key_callback(struct GLFWwindow* window, 
                int key, 
                int scancode, 
                int action, 
                int mods)
{
  for (uint32_t i = 0; i < m_num_callbacks; ++i) 
  {
    m_input_callbacks[i].key_callback(window, 
                                      key,
                                      scancode, 
                                      action,
                                      mods);
  }
}

 
void 
gs_mouse_pos_callback(struct GLFWwindow* window, 
                           double xpos, 
                           double ypos)
{
  for (uint32_t i = 0; i < m_num_callbacks; ++i) 
  {
    m_input_callbacks[i].mouse_pos_callback(window,
                                      xpos, 
                                      ypos);
  }
}


void 
gs_mouse_button_callback(struct GLFWwindow* window, 
                         int button, 
                         int action, 
                         int mods)
{
  for (uint32_t i = 0; i < m_num_callbacks; ++i) 
  {
    m_input_callbacks[i].mouse_button_callback(window,
                                               button, 
                                               action, 
                                               mods);
  }
}
