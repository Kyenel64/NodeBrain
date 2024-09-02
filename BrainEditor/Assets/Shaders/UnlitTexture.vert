#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec2 outUV;

layout (push_constant) uniform constants
{
    mat4 ViewMatrix;
    mat4 ModelMatrix;
} PushConstants;


void main()
{
    gl_Position = PushConstants.ViewMatrix * PushConstants.ModelMatrix * inPosition;
    outUV = inUV;
}
