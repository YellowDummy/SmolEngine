#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{

}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    color = vec4(hdrColor, 1.0);
}

