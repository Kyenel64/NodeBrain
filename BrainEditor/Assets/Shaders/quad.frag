#version 450

layout(location = 0) in vec4 inFragColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in flat int inTextureIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D textures[16];

void main()
{
    vec4 texColor = inFragColor;

    switch(inTextureIndex)
    {
        case 0:  texColor *= texture(textures[0],  inUV); break;
        case 1:  texColor *= texture(textures[1],  inUV); break;
        case 2:  texColor *= texture(textures[2],  inUV); break;
        case 3:  texColor *= texture(textures[3],  inUV); break;
        case 4:  texColor *= texture(textures[4],  inUV); break;
        case 5:  texColor *= texture(textures[5],  inUV); break;
        case 6:  texColor *= texture(textures[6],  inUV); break;
        case 7:  texColor *= texture(textures[7],  inUV); break;
        case 8:  texColor *= texture(textures[8],  inUV); break;
        case 9:  texColor *= texture(textures[9],  inUV); break;
        case 10: texColor *= texture(textures[10], inUV); break;
        case 11: texColor *= texture(textures[11], inUV); break;
        case 12: texColor *= texture(textures[12], inUV); break;
        case 13: texColor *= texture(textures[13], inUV); break;
        case 14: texColor *= texture(textures[14], inUV); break;
        case 15: texColor *= texture(textures[15], inUV); break;
    }

    outColor = texColor;
}
