#version 460 core

layout(location = 0) in vec3 v_bitangent;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_tangent;
layout(location = 3) in vec2 v_texCoord;
layout(location = 4) in vec3 v_incident;

layout (binding = 1) uniform samplerCube samplerIrradiance;

layout (location = 0) out vec4 outColor;

void main()
{

}