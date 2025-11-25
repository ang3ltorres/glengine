#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D image;

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    float shininess;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    vec4 texColor = texture(image, TexCoord);
    if(texColor.a < 0.1)
        discard;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.color;

    vec3 result = (ambient + diffuse + specular) * texColor.rgb * Color.rgb;
    FragColor = vec4(result, texColor.a * Color.a);
}
