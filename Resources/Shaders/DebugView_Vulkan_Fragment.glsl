#version 450

layout (location = 0) in vec2 inUV;
layout (binding = 1) uniform sampler2D shadowMap;

layout(push_constant) uniform DebugData
{
	uint  showOmni;
	uint  showMRT;

    uint  mrtAttachmentIndex;
};

layout (location = 0) out vec4 outFragColor;

float LinearizeDepth(float depth)
{
  float n = 1.0; // camera z near
  float f = 128.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main() 
{
    if(showOmni == 1)
    {
        float depth = texture(shadowMap, inUV).r;
        outFragColor = vec4(vec3(1.0 - LinearizeDepth(depth)), 1.0);;
        return;
    }

	outFragColor = vec4(1, 1, 1, 1);
}