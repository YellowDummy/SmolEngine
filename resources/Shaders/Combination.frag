#version 450
layout (location = 0) in vec2 inUV;
layout (binding = 0) uniform sampler2D colorSampler;
layout (binding = 1) uniform sampler2D bloomSampler;
layout (binding = 2) uniform sampler2D blurSampler;

layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform ConstantData
{
    uint state;
};

void main() 
{
    vec4 color = vec4(1.0);
    switch(state)
    {
        case 0:
        {
            color = texture(colorSampler, inUV);
            break;
        }
        case 1:
        {
            vec3 hdrColor = texture(colorSampler, inUV).rgb;
            vec3 bloomColor = texture(bloomSampler, inUV).rgb;
            color = vec4(hdrColor + bloomColor, 1.0); // additive blending
            break;
        }
        case 2:
        {
            vec3 hdrColor = texture(colorSampler, inUV).rgb;
            vec3 bloomColor = texture(bloomSampler, inUV).rgb;
            vec3 blurColor = texture(blurSampler, inUV).rgb;
            color = vec4(hdrColor + bloomColor + blurColor, 1.0); // additive blending
            break;
        }
    }

    outFragColor = color;
}