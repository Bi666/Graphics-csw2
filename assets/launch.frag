#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec2 v2fTextureCoords;

layout (location=0) out vec3 oColor;

layout (location=2) uniform vec3 uLightDir; //direction of the light to all surfaces
layout (location=3) uniform vec3 uLightDiffuse;
layout (location=4) uniform vec3 uSceneAmbient;
//layout (location=5) uniform vec3 uBaseColor;
layout (binding=0) uniform sampler2D uTexture;


void main()
{
	// vec3 normal = normalize(v2fNormal);
	vec3 normal = normalize(v2fNormal);

	float nDotL = max(0.0 , dot(normal, uLightDir));

	//oColor = uBaseColor * v2fColor;
	//oColor = texture(uTexture, v2fTextureCoords).rgb;
	oColor = (uSceneAmbient + nDotL * uLightDiffuse) * v2fColor;
	//oColor = texture(uTexture, v2fTextureCoords).rgb * (uSceneAmbient + nDotL * uLightDiffuse); //might not work

}