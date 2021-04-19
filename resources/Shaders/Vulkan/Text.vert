#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

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

layout(push_constant) uniform pc
{
	mat4 model;
	vec4 color;
	uint textureID;
};

layout (location = 0) out vec2 v_UV;
layout (location = 1) out vec4 v_color;
layout (location = 2) out uint v_texID;

void main() 
{
	v_UV = inUV;
	v_texID = textureID;
	v_color = color;
	vec4 pos = model * vec4(inPos, 1.0);
	gl_Position = sceneData.projection * sceneData.view * pos;
}