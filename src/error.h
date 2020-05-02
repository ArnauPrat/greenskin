


#ifndef _GS_ERROR_H_
#define _GS_ERROR_H_

#include "platform.h"
#include <assert.h>

typedef enum gs_error_t
{
  E_SUCCESS = 0,
  E_IO_FILE_NOT_FOUND, 
  E_IO_UNEXPECTED_INPUT_ERROR, 

  E_RESOURCES_RDIR_PATH_NAME_ERROR, 
  E_RESOURCES_FILE_PATH_NAME_ERROR, 

  E_RENDERER_RESOURCE_ALLOCATION_ERROR, 
  E_RENDERER_INITIALIZATION_ERROR, 
  E_RENDERER_RUNTIME_ERROR,
  E_RENDERER_VULKAN_ERROR,
  E_RENDERER_GUI_ERROR,

  E_TASKING_MAX_ECONTEXTS_ERROR,
  E_TASKING_MAX_TCONTEXTS_ERROR,

  E_MEMORY_INSUFFICIENT_BUFFER_SIZE_ERROR,

  E_NUM_ERRORS
} gs_error_t;

extern const char* error_messages[(uint32_t)(E_NUM_ERRORS)];

#endif /* ifndef _GS_ERROR_H_ */