#version 450 core

layout(location = 0) in vec3 a_Position;
layout(push_constant) uniform CameraData
{
	mat4 u_Projection;
	mat4 u_Model;
	float u_Exposure;
	float u_Gamma;
};

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out float v_Gamma;
layout(location = 2) out float v_Exposure;

void main()
{
	v_WorldPos = a_Position;
	v_Gamma = u_Gamma;
	v_Exposure = u_Exposure;

	v_WorldPos.xy *= -1.0;
	
	gl_Position = u_Projection * u_Model * vec4(a_Position.xyz, 1);
}
