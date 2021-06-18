#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec3 v_pos;
layout(location = 2) in vec3 v_normals;
layout(location = 3) in vec2 v_uv;
layout(location = 4) flat in uint v_texIndex;

layout (binding = 0) uniform sampler2D texturesMap[4096];

void main()
{
	vec4 texColor = texture(texturesMap[v_texIndex], v_uv);
	color = texColor * vec4(v_color.rgb, texColor.a);
}