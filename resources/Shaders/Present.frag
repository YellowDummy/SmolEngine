#version 450 core
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragColor;

layout (binding = 0) uniform sampler2D renderer;
layout (binding = 1) uniform sampler2D renderer2D;

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
            color = texture(renderer, inUV);
            break;
        }
        case 1:
        {
            color = texture(renderer2D, inUV);
            break;
        }
        case 2:
        {
            vec3 color1 = texture(renderer, inUV).rgb;
            vec3 color2 = texture(renderer2D, inUV).rgb;
            color = vec4(color1 + color2, 1.0);
            break;
        }
    }

    outFragColor = color;
}