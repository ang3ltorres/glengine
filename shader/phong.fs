#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D image;

struct Light
{
	vec4 position;
	vec4 color;
};

layout(std140, binding = 2) uniform GPU_UBO_LIGHT
{
	Light lights[8];
	int LightCurrentCount;
};

struct Material
{
	float shininess;
	vec3 specular;
};

uniform Material material;
uniform vec3 viewPos;

void main()
{
	vec3 resultColor = vec3(0.0);

	for (int i = 0; i < LightCurrentCount; i++)
	{
		vec3 lightColor = lights[i].color.rgb;
		vec3 lightPos   = lights[i].position.xzy;

		// Ambient
		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColor;

		// Diffuse
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		// Specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = material.specular * spec * lightColor;

		resultColor += ambient + diffuse + specular;
	}

	vec4 texColor = texture(image, TexCoord);

	if (texColor.a < 0.1)
		discard;

	resultColor *= texColor.rgb * Color.rgb;
	FragColor = vec4(resultColor, texColor.a * Color.a);
}
