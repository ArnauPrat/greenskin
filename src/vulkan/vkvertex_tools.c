

#include "../vertex.h"
#include "vkvertex_tools.h"


VkVertexInputBindingDescription 
get_binding_description() 
{
  VkVertexInputBindingDescription bindingDescription = {};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(gs_vertex_t);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

void 
get_attribute_descriptions(VkVertexInputAttributeDescription* input_attr_desc) 
{
  input_attr_desc[0].binding = 0;
  input_attr_desc[0].location = 0;
  input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  input_attr_desc[0].offset = offsetof(gs_vertex_t, m_position);

  input_attr_desc[1].binding = 0;
  input_attr_desc[1].location = 1;
  input_attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  input_attr_desc[1].offset = offsetof(gs_vertex_t, m_color);

  input_attr_desc[2].binding = 0;
  input_attr_desc[2].location = 2;
  input_attr_desc[2].format = VK_FORMAT_R32G32_SFLOAT;
  input_attr_desc[2].offset = offsetof(gs_vertex_t, m_tex_coord);
  return;
}
