#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 outUV;

struct Vertex
{
    vec3 Position;
    float UVX;
    vec3 Normal;
    float UVY;
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

layout (set = 0, binding = 0) uniform UBO
{
    mat4 ModelMatrix;
} ubo;

void main()
{
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = PushConstants.ViewMatrix * ubo.ModelMatrix * vec4(v.Position, 1.0f);
    outUV = vec2(v.UVX, v.UVY);
}
