#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_UV;
layout(location = 4) in ivec4 a_BoneIDs;
layout(location = 5) in vec4 a_Weight;

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


layout (location = 0) out vec2 v_UV;
layout (location = 1) out vec3 v_Pos;

layout(push_constant) uniform ConstantData
{
	mat4 model;
};

void main() 
{
   vec4 pos = model * vec4(a_Position, 1.0);
   v_Pos = pos.xyz;
   v_UV = a_UV;

   gl_Position =  sceneData.projection * sceneData.view * pos;   

}