#version 450
#extension GL_EXT_buffer_reference : require


layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outUV;
layout (location = 2) out flat int outTextureIndex;

struct Vertex
{
    vec3 Position;
    float UVX;
    vec3 Normal;
    float UVY;
    vec4 Color;
    ivec4 Test;
};

layout (std430, buffer_reference, buffer_reference_align = 16) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout (push_constant) uniform constants
{
    mat4 ViewMatrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main()
{
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = PushConstants.ViewMatrix * vec4(v.Position, 1.0f);
    outColor = v.Color;
    outUV = vec2(v.UVX, v.UVY);
    outTextureIndex = v.Test.x;
}
