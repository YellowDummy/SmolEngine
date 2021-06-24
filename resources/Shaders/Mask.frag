#version 460

layout (location = 0) in vec2 inUV;
layout (binding = 0) uniform sampler2D maskSampler;

layout(push_constant) uniform ConstantData
{
    float mask_intensity;
};

layout (location = 0) out vec4 outFragColor;

void main() 
{
    vec4 color = texture(maskSampler, inUV);
    color.a = mask_intensity;
    outFragColor = color;
}