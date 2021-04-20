#version 450 core

layout(location = 0) in vec3 a_Position;

layout(push_constant) uniform DebugData
{
    mat4 u_Transform;
    vec4 u_Color;
};

layout(location = 1) out vec4 v_Color;

void main()
{
    v_Color = u_Color;
	gl_Position = u_Transform * vec4(a_Position, 1);
}