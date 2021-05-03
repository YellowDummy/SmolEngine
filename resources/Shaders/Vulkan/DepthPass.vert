#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

struct InstanceData
{
	uint matID;
	uint isAnimated;
	uint animOffset;
	uint entityID;
	mat4 model;
};

layout(std140, binding = 25) readonly buffer InstancesBuffer
{   
	InstanceData instances[];
};

layout(std430, binding = 28) readonly buffer JointMatrices
{
	mat4 joints[];
};

layout(push_constant) uniform PushConsts
 {
	mat4 depthMVP;
	uint dataOffset;
};

out gl_PerVertex 
{
    vec4 gl_Position;   
};

 
void main()
{
	const uint instanceID = dataOffset + gl_InstanceIndex;
	const mat4 model = instances[instanceID].model;
	const uint animOffset = instances[instanceID].animOffset;
	const bool isAnimated = bool(instances[instanceID].isAnimated);

	mat4 skinMat = mat4(1.0);
	if(isAnimated)
	{
		skinMat = 
		a_Weight.x * joints[animOffset + uint(a_BoneIDs.x)] +
		a_Weight.y * joints[animOffset + uint(a_BoneIDs.y)] +
		a_Weight.z * joints[animOffset + uint(a_BoneIDs.z)] +
		a_Weight.w * joints[animOffset + uint(a_BoneIDs.w)];
	}

	const vec4 pos = (model * skinMat) * vec4(a_Position, 1.0);
	gl_Position =  depthMVP * pos;
}