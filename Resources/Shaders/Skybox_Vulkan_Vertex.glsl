#version 450 core

layout(location = 0) in vec3 a_Position;
layout(push_constant) uniform CameraData
{
	mat4 u_Projection;
	mat4 u_Model;
};

layout(location = 1) out vec3 v_WorldPos;

void main()
{
	v_WorldPos = a_Position;
	
	gl_Position = u_Projection * u_Model * vec4(a_Position.xyz, 1);
}
