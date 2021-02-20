#version 450

layout (binding = 24) uniform sampler2D texturesMap[4096];

layout (location = 0) in vec2 inUV;
layout (location = 2) in vec4 inColor;
layout (location = 3) flat in uint inColorTexID;
layout (location = 4) flat in uint inUseAlbedro;

void main() 
{	
	float alpha = inUseAlbedro == 1 ? texture(texturesMap[inColorTexID], inUV).a: inColor.a;
	if (alpha < 0.5) {
		discard;
	}
}