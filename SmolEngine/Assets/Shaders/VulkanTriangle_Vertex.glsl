#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCood;

layout(location = 3) out vec4 v_Color;
layout(location = 4) out vec4 v_Pos;
layout(location = 5) out vec2 v_TextCood;

layout (std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjection;
};

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1);

    v_Pos = u_ViewProjection * vec4(a_Position, 1);
    v_Color = a_Color;
    v_TextCood = a_TextCood;
}