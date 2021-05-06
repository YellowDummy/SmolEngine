#version 450

layout (binding = 1) uniform sampler2D samplerColor[100];

layout (location = 0) in vec2 v_UV;
layout (location = 1) in vec4 v_color;
layout (location = 2) flat in uint v_texID;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    float distance = texture(samplerColor[v_texID], v_UV).a;
    float smoothWidth = fwidth(distance);	
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
	vec3 rgb = vec3(alpha);																 
    outFragColor = vec4(rgb, alpha) * v_color;
}