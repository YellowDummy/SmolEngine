#version 450

// in
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in vec4 a_Color;

// out
layout (location = 0) out vec4 outPos;
layout (location = 1) out vec3 outLightPos;

struct ShaderData
{
	mat4 model;
	vec4 data;
};

struct SceneData
{
	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 camPos;
	vec4 params;
};

layout(std140, binding = 25) readonly buffer ShaderDataBuffer
{   
	ShaderData instances[];
};

layout (std140, binding = 27) uniform SceneDataBuffer
{
    SceneData sceneData;
};

layout(push_constant) uniform PushConsts 
{
	mat4 view;
    mat4 proj;
    vec4 lightPos;
    uint dataOffset;
};

out gl_PerVertex 
{
	vec4 gl_Position;
};

mat4 translate(mat4 m, vec3 v)
{
    mat4 result = m;
    result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return result;
}

void main()
{
    mat4 model = instances[dataOffset + gl_InstanceIndex].model;
    outPos = vec4(a_Position, 1.0);
    outLightPos = vec4(lightPos).xyz;

    gl_Position = view * model * vec4(a_Position, 1.0);
}