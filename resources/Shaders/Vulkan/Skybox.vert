#version 450 core

layout(location = 0) in vec3 a_Position;

struct SceneData
{
	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 params;
};

layout (std140, binding = 27) uniform SceneDataBuffer
{
    SceneData sceneData;
};

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out float v_Gamma;
layout(location = 2) out float v_Exposure;

void main()
{
	v_WorldPos = a_Position;
	v_Gamma = sceneData.params.x;
	v_Exposure = sceneData.params.y;

	v_WorldPos.xy *= -1.0;
	
	gl_Position = sceneData.projection * sceneData.skyBoxMatrix * vec4(a_Position.xyz, 1);
}
