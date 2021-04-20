#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) in vec4 v_Color;

void main()
{
	color = v_Color;
}