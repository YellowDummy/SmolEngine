#version 450
layout (location = 0) in vec2 inUV;
layout (binding = 0) uniform sampler2D colorSampler;
layout (binding = 1) uniform sampler2D bloomSampler;

layout (location = 0) out vec4 outFragColor;


layout(push_constant) uniform ConstantData
{
    uint hdr;
};

void main() 
{
    if(hdr == 1)
    {
        vec3 hdrColor = texture(colorSampler, inUV).rgb;
        vec3 bloomColor = texture(bloomSampler, inUV).rgb;
        vec3 color = hdrColor + bloomColor; // additive blending
        outFragColor = vec4(color, 1.0);
        return;
    }

    outFragColor = texture(colorSampler, inUV);
}