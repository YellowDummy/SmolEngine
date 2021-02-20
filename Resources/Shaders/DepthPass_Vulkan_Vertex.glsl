#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

#define SHADOW_MAP_CASCADE_COUNT 4

struct ShaderData
{
	mat4 model;
	vec4 data;
};

struct MaterialData
{
   ivec4 TextureStates;
   ivec4 TextureStates_2;

   ivec4 TextureIndexes;
   ivec4 TextureIndexes_2;

   vec4  PBRValues;
};

layout(std140, binding = 25) readonly buffer ShaderDataBuffer
{   
	ShaderData instances[];
};

layout(std140, binding = 26) readonly buffer ObjectBuffer
{   
	MaterialData materials[];

};

struct CascadeViewProjMat
{
	mat4[SHADOW_MAP_CASCADE_COUNT] viewProj;
};

layout (binding = 1) uniform UBO 
{
	CascadeViewProjMat cascadeViewProjMat;
};

layout(push_constant) uniform PushConsts {
	uint dataOffset;
	uint cascadeIndex;
};

layout (location = 0) out vec2 outUV;
layout (location = 2) out vec4 outColor;
layout (location = 3) out uint outColorTexID;
layout (location = 4) out uint outUseAlbedro;

out gl_PerVertex {
	vec4 gl_Position;   
};

void main()
{
	mat4 model = instances[dataOffset + gl_InstanceIndex].model;
	int materialIndex = int(instances[dataOffset + gl_InstanceIndex].data.x);

	outUseAlbedro = materials[materialIndex].TextureStates.x;
	outColorTexID = materials[materialIndex].TextureIndexes.x;

	vec3 pos = vec4(model * vec4(a_Position, 1)).xyz;
	outUV = a_UV;
	outColor = a_Color;

	gl_Position =  cascadeViewProjMat.viewProj[cascadeIndex] * vec4(pos, 1.0);
}