

#include "log.h"
#include "error.h"

#include <stdlib.h>


const char* error_messages[(uint32_t)(E_NUM_ERRORS)] = 
{          
  "Success",
  "IO:File Not Found Error",
  "IO:Unexpected Input Error",
  "RESOURCES:Resources directory path name error", 
  "RESOURCES:Resources file path name error", 
  "RENDERER:Resource Allocation Error",
  "RENDERER:Unexpected Initialization Error",
  "RENDERER:Unexpected Runtime Error",
  "RENDERER:Vulkan API Error",
  "RENDERER:Gui Error",
  "TASKING:Max execution contexts exceeded",
  "TASKING:Max task contexts exceeded",
  "MEMORY:Insuficient buffer size Error"
};                                                                              
