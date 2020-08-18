//

#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextureCoordinates;
layout(location = 3) in float a_TextureIndex;
layout(location = 4) in float a_RepeatValue;

uniform mat4 u_ViewProjection;

out vec2 v_TextureCoordinates;
out vec4 v_Color;
out float v_TextureIndex;
out float v_RepeatValue;

void main() 
{
   v_RepeatValue = a_RepeatValue;
   v_Color = a_Color;
   v_TextureIndex = a_TextureIndex;
   v_TextureCoordinates = a_TextureCoordinates;
   gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TextureCoordinates;
in float v_TextureIndex;
in float v_RepeatValue;

uniform sampler2D u_Textures[32];

void main() 
{
  color = texture(u_Textures[int(v_TextureIndex)], v_TextureCoordinates * v_RepeatValue) * v_Color;
}