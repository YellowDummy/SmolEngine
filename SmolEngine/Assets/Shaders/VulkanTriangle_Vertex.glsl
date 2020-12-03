#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 v_Color;

void main()
{
    vec4 pos = vec4(a_Position, 1.0);
    gl_Position = pos;

    v_Color = a_Color;
}