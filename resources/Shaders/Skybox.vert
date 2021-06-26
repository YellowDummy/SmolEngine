#version 450 core

layout(location = 0) in vec3 a_Position;

layout (std140, binding = 27) uniform SceneBuffer
{
	float nearClip;
    float farClip;
    float exoposure;
    float pad;

	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 ambientColor;

} sceneData;

layout(location = 0) out vec3 v_WorldPos;

void main()
{
	v_WorldPos = a_Position;
	v_WorldPos.xy *= -1.0;
	
	gl_Position = sceneData.projection * sceneData.skyBoxMatrix * vec4(a_Position.xyz, 1);
}
