#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform constants
{
    mat4 ViewMatrix;
    mat4 ModelMatrix;
} PushConstants;

layout (set = 0, binding = 0) uniform MaterialUBO
{
    vec4 Color;
} materialUBO;

void main()
{
    gl_Position = PushConstants.ViewMatrix * PushConstants.ModelMatrix * inPosition;
    outColor = materialUBO.Color;
}
