#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Material
{
  vec3 color;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4     projmodelview;
    Material material;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.projmodelview * vec4(inPosition, 1.0);
    fragColor = inColor*ubo.material.color;
}
