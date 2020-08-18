//

#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

float offset = -0.1;
out vec3 v_Position;
out vec4 v_Color;

void main() 
{
   gl_Position = u_ViewProjection * u_Transform * vec4(a_Position - offset, 1.0);
   v_Color = a_Color;
   v_Position = a_Position;
}

#type fragment
#version 410 core
layout(location = 0) out vec4 color;
uniform vec4 u_Color;

in vec3 v_Position;
in vec4 v_Color;

void main() 
{
   if(u_Color != 0)
   {
      color = u_Color;
   }
   else
   {
     color = v_Color;
   }
}
