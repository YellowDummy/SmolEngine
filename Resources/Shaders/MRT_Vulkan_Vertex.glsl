#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

layout(push_constant) uniform CameraData
{
	mat4 projection;
	mat4 model;
	mat4 view;

	float nearPlane;
	float farPlane;
};

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec4 outTangent;
layout (location = 4) out float outNearPlane;
layout (location = 5) out float outFarPlane;

void main()
{
	outWorldPos = vec3(model * vec4(a_Position, 1.0));

	// Normal in view space
	outNormal = mat3(model) * a_Normal;

	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outUV = a_UV;
	outNearPlane = nearPlane;
	outFarPlane = farPlane;

	gl_Position =  projection * view * vec4(outWorldPos, 1.0);

	return;

	vec3 locPos = vec3(model * vec4(a_Position, 1.0));
	
	outWorldPos = locPos;
	outNormal = mat3(model) * a_Normal;
	outTangent = vec4(mat3(model) * a_Tangent.xyz, a_Tangent.w);
	outUV = a_UV;
	outNearPlane = nearPlane;
	outFarPlane = farPlane;
}