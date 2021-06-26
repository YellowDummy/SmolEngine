#version 460

layout(location = 0) in vec3 a_Position;

layout(push_constant) uniform PushConsts 
{
	mat4 view;
    mat4 projection;
	uint index;
};

layout(location = 0) out vec3 v_WorldPos;

void main()
{
	v_WorldPos = a_Position;
	v_WorldPos.xy *= -1.0;
	
	gl_Position = projection * view * vec4(a_Position.xyz, 1);
}
