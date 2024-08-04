#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec4 outColor;

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

layout (set = 0, binding = 0) uniform PerObjectUBO
{
    mat4 ModelMatrix;
} perObjectUBO;

layout (set = 0, binding = 1) uniform MaterialUBO
{
    vec4 Color;
} materialUBO;

void main()
{
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = PushConstants.ViewMatrix * perObjectUBO.ModelMatrix * vec4(v.Position, 1.0f);
    outColor = materialUBO.Color;
}
