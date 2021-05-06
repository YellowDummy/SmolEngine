#version 460 core

layout (location = 0) in vec2 v_UV;
layout (location = 0) out vec4 outColor;

void main() 
{
    float divisions = 20.0;
    float thickness = 0.005;
    float delta = 0.05 / 2.0;
    
    float x = fract(v_UV.x / (1.0 / divisions));
    float xdelta = fwidth(x) * 2.5;
    x = smoothstep(x - xdelta, x + xdelta, thickness);
    
    float y = fract(v_UV.y / (1.0 / divisions));
    float ydelta = fwidth(y) * 2.5;
    y = smoothstep(y - ydelta, y + ydelta, thickness);
    
    float c = clamp(x + y, 0.1, 1.0);
    if(c < 0.2)
    {
        discard;
    }

    outColor = vec4(c, c, c, 0.0);
}