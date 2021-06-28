#version 460 core

layout (location = 0) in vec2 v_UV;
layout (location = 1) in vec3 v_Pos;

layout (location = 0) out vec4 color;

float when_gt(float x, float y) {
  return max(sign(x - y), 0.0);
}

vec2 when_gt(vec2 x, vec2 y) {
  return max(sign(x - y), 0.0);
}

vec3 when_gt(vec3 x, vec3 y) {
  return max(sign(x - y), 0.0);
}

vec4 when_gt(vec4 x, vec4 y) {
  return max(sign(x - y), 0.0);
}

float when_le(float x, float y) {
  return 1.0 - when_gt(x, y);
}

vec2 when_le(vec2 x, vec2 y) {
  return 1.0 - when_gt(x, y);
}

vec3 when_le(vec3 x, vec3 y) {
  return 1.0 - when_gt(x, y);
}

vec4 when_le(vec4 x, vec4 y) {
  return 1.0 - when_gt(x, y);
}

float grid(vec3 pos, vec3 axis, float size) {
    float width = 1.0;

    // Grid size
    vec3 tile = pos / size;

    // Grid centered gradient
    vec3 level = abs(fract(tile) - 0.5);

    // Derivative (crisp line)
    vec3 deri = fwidth(tile);

    vec3 grid3D = clamp((level - deri * (width - 1.0)) / deri, 0.0, 1.0);

    // Shorter syntax but pow(0.0) fails on some GPUs
    // float lines = float(length(axis) > 0.0) * pow(grid3D.x, axis.x) * pow(grid3D.y, axis.y) * pow(grid3D.z, axis.z);

    float lines = float(length(axis) > 0.0)
        * (when_gt(axis.x, 0.0) * grid3D.x + when_le(axis.x, 0.0))
        * (when_gt(axis.y, 0.0) * grid3D.y + when_le(axis.y, 0.0))
        * (when_gt(axis.z, 0.0) * grid3D.z + when_le(axis.z, 0.0));
    return 1.0 - lines;
}

void main() 
{
   float lines = grid(v_Pos, vec3(1.0, 1.0, 1.0), 5.0);
   if(lines < 0.2)
   {
       discard;
   }
   
   color = vec4(vec3(lines), 0.0); // aplha need to be 0 (see lighting.frag)
}