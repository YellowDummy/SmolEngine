#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 v_Color;
layout(location = 2) out vec4 v_Pos;

layout (std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjection;
};

void main()
{
    vec4 pos = vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * pos;

    v_Pos = u_ViewProjection * pos;
    v_Color = a_Color;
}