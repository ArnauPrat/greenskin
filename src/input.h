

#ifndef _GS_INPUT_H_
#define _GS_INPUT_H_


#include "platform.h"

struct GLFWwindow;

typedef void (*gs_process_key_t)(struct GLFWwindow*, int key, int scancode, int action, int mods);
typedef void (*gs_process_mouse_pos_t)(struct GLFWwindow*, double xpos, double ypos);
typedef void (*gs_process_mouse_button_t)(struct GLFWwindow*, int button, int action, int mods);

typedef struct gs_input_callbacks_t
{
  gs_process_key_t          key_callback;
  gs_process_mouse_pos_t    mouse_pos_callback;
  gs_process_mouse_button_t mouse_button_callback;
} gs_input_callbacks_t;


void
gs_input_init();

void
gs_input_release();

uint32_t
gs_input_register_callbacks(gs_input_callbacks_t callbacks);

void
gs_input_unregister_callbacks();


void 
gs_key_callback(struct GLFWwindow* window, 
                int key, 
                int scancode, 
                int action, 
                int mods);


void 
gs_mouse_pos_callback(struct GLFWwindow* window, 
                           double xpos, 
                           double ypos);


void 
gs_mouse_button_callback(struct GLFWwindow* window, 
                         int button, 
                         int action, 
                         int mods);

#endif
