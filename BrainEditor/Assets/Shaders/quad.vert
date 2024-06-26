#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;

struct Vertex
{
    vec3 Position;
    float UVX;
    vec3 Normal;
    float UVY;
    vec4 Color;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout (set = 0, binding = 0) uniform GlobalUBO1
{
    vec4 Color1;
} globalUBO1;

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
}