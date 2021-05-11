#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

struct Instance
{
	mat4 model;
	vec4 color;
	ivec4 params; // x - texture index
};

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

layout(std140, binding = 1) readonly buffer ShaderDataBuffer
{   
	Instance data[];
};

layout(push_constant) uniform ConstantData
{
	uint dataOffset;
};

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec3 v_pos;
layout(location = 2) out vec3 v_normals;
layout(location = 3) out vec2 v_uv;
layout(location = 4) out uint v_texIndex;

void main()
{
	uint index = dataOffset + gl_InstanceIndex;
	mat4 model = data[index].model;

	v_color = data[index].color;
	v_texIndex = data[index].params.x;
	v_pos = vec3(model * vec4(a_Position, 1.0));
	v_normals = mat3(model) * a_Normal;
	v_uv = a_UV;

    gl_Position =  sceneData.projection * sceneData.view * vec4(v_pos, 1.0);
}