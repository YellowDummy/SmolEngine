#version 460

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D colorSampler;
layout (binding = 1) uniform sampler2D ppSampler;

layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform ConstantData
{
    uint ppState; //post processing
};

void main() 
{
    vec4 color = texture(colorSampler, inUV);
    switch(ppState)
    {
        case 1:
        {
            vec4 blendColor = texture(ppSampler, inUV);
            color += vec4(blendColor.rgb, 1.0);
            break;
        }
    }

    outFragColor = color;
}