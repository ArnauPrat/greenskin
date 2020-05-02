

#ifndef _GS_VKRENDERER_GUI_H_
#define _GS_VKRENDERER_GUI_H_

#include "../../platform.h"

void
gsvk_gui_init();

void
gsvk_gui_recreate();

void
gsvk_gui_begin_frame();

void
gsvk_gui_build_commands(uint32_t index);

void
gsvk_gui_release();
  
#endif
