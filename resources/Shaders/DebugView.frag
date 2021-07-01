#version 460

layout (location = 0) in vec2 inUV;

layout (binding = 1) uniform sampler2D shadowMap;
layout (binding = 5) uniform sampler2D albedroMap;
layout (binding = 6) uniform sampler2D positionsMap;
layout (binding = 7) uniform sampler2D normalsMap;
layout (binding = 8) uniform sampler2D materialsMap;
layout (binding = 9) uniform sampler2D shadowCoordMap;

layout (location = 0) out vec4 outFragColor;
layout(push_constant) uniform ConstantData
{
  uint state;
};

void main() 
{
  vec4 color = vec4(0.2, 0.2, 0.2, 1.0);
  switch(state)
  {
    case 1:
    {
      color = vec4(texture(albedroMap, inUV).rgb, 1.0);
      break;
    }

    case 2:
    {
      color = vec4(texture(positionsMap, inUV).rgb, 1.0);
      break;
    }

    case 3:
    {
      color = vec4(texture(normalsMap, inUV).rgb, 1.0);
      break;
    }

    case 4:
    {
      vec2 metalnessRoughness = texture(materialsMap, inUV).xy;
      color = vec4(metalnessRoughness, 1.0, 1.0);
      break;
    }

    case 5:
    {
      color = vec4(vec3(texture(materialsMap, inUV).w), 1.0);
      break;
    }

    case 6:
    {
      color = vec4(vec3(texture(shadowMap, inUV).r), 1.0);
      break;
    }

    case 7:
    {
      color = vec4(texture(shadowCoordMap, inUV).rgb, 1.0);
      break;
    }

    case 8:
    {
      float ao = texture(materialsMap, inUV).z;
      color = vec4(vec3(ao), 1);
      break;
    }
  }

  outFragColor = color;
}