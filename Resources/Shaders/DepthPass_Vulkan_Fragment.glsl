#version 450

layout (binding = 0) uniform sampler2D colorMap;
layout (location = 0) in vec2 inUV;

void main() 
{	
	float alpha = texture(colorMap, inUV).a;
	if (alpha < 0.5) {
		discard;
	}
}