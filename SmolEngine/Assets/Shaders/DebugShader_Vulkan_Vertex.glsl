#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(push_constant) uniform DebugData
{
    mat4 u_ViewProjection;
	mat4 u_Transform;
};

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}