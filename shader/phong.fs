#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D image;

layout (std140, binding = 0) uniform UBO_Shared_Camera
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;
	vec4 CameraPosition;
};

struct Light
{
	vec4 position;
	vec4 color;
	vec4 attenuation;
};

struct Material
{
	vec3 specular;
	float shininess;
};

layout (std140, binding = 2) uniform UBO_Shared_Light
{
	Light lights[8];
	Material material;
	int LightCount;
	bool Enable;
};

void main()
{
	vec3 resultColor = vec3(0.0);
	vec3 viewPos = CameraPosition.xyz;

	if (Enable)
	{
		for (int i = 0; i < LightCount; i++)
		{
			vec3 lightColor = lights[i].color.rgb;
			vec3 lightPos   = lights[i].position.xzy;

			vec4 attenuationParams = lights[i].attenuation;
			
			// Ambient
			float ambientStrength = 0.1;
			vec3  ambient         = ambientStrength * lightColor;

			// Diffuse
			vec3  norm     = normalize(Normal);
			vec3  lightDir = normalize(lightPos - FragPos);
			float diff     = max(dot(norm, lightDir), 0.0);
			vec3  diffuse  = diff * lightColor;

			// Specular
			vec3  viewDir    = normalize(viewPos - FragPos);
			vec3  reflectDir = reflect(-lightDir, norm);
			float spec       = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3  specular   = material.specular * spec * lightColor;

			// Attenuation
			float distance    = length(lightPos - FragPos);
			float attenuation = 1.0 / (attenuationParams.x + attenuationParams.y * distance + attenuationParams.z * (distance * distance));    

			ambient  *= attenuation; 
			diffuse  *= attenuation;
			specular *= attenuation;

			resultColor += ambient + diffuse + specular;
		}
	}
	else
		resultColor = vec3(1.0);

	vec4 texColor = texture(image, TexCoord);

	if (texColor.a < 0.1)
		discard;

	resultColor *= texColor.rgb * Color.rgb;
	FragColor = vec4(resultColor, texColor.a * Color.a);
}
