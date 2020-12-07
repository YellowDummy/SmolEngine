#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec4 v_Color;
layout(location = 2) in vec4 v_Pos;

// only one push_constant block allowed per stage
// max size is 128 bytes
layout(push_constant) uniform TestData
{
    vec3 value;
};

void main()
{
    //float ambient = 0.1;
    //vec3 LightPos = vec3(1, 0, 0);
    //vec4 LightColor = vec4(0.5, 0.6, 0.2, 1.0);

    //float intensity = ( 1.0 / length(v_Pos.xy - LightPos.xy)) * 1.0;
	//intensity = atan(intensity, sqrt(length(v_Pos.xy - LightPos.xy)));

    //vec4 tempColor = v_Color * ambient;
    //tempColor.rgb += (LightColor.rgb * intensity);
   // float value = atan(1.0, sqrt(length(v_Pos.xy * v_Color.xy)));
    
    color = vec4(v_Color.rgb * value, v_Color.a);
}