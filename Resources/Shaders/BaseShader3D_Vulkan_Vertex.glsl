#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in vec4 a_Color;

layout(push_constant) uniform CameraData
{
	mat4 u_ViewProjection;
	mat4 u_Trans;
	vec4 u_Color;
	vec3 u_Camera;
};

layout(location = 4) out vec4 v_Color;
layout(location = 5) out vec2 v_UV;
layout(location = 6) out vec3 v_Normal;
layout(location = 7) out vec3 v_WorldPos;
layout(location = 8) out vec3 v_Camera;

void main()
{
	v_Color = u_Color;
	v_UV = a_UV;
	v_Normal = a_Normal;
	v_WorldPos = a_Position;
	v_Camera = u_Camera;

	gl_Position = u_ViewProjection * u_Trans * vec4(a_Position, 1);
}