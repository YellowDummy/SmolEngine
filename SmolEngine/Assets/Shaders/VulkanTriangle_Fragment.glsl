#version 450 core

layout(location = 0) out vec4 color;

layout(location = 3) in vec4 v_Color;
layout(location = 4) in vec4 v_Pos;
layout(location = 5) in vec2 v_TextCood;

// only one push_constant block allowed per stage
// max size is 128 bytes
layout(push_constant) uniform TestData
{
    vec3 value;
};

layout(binding = 1) uniform sampler2D myTex[2];

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
    
    vec4 textureColor = texture(myTex[1], v_TextCood);
    color = vec4(textureColor.rgb * value, textureColor.a);
}