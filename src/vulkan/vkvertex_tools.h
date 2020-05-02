

#ifndef _GS_VKVERTEX_TOOLS_H_
#define _GS_VKVERTEX_TOOLS_H_ 

#include "vkrenderer.h"

VkVertexInputBindingDescription 
get_binding_description(); 

void
get_attribute_descriptions(VkVertexInputAttributeDescription* descriptors);  

#endif 
