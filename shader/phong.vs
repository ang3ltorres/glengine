#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aColor;

uniform float uTime;

out vec2 TexCoord;
out vec4 Color;
out vec3 FragPos;
out vec3 Normal;

struct InstanceData
{
	mat4 Model;
	vec4 Color;
};

layout (std430, binding = 1) buffer SSBO_Data
{
	InstanceData instances[];
};

//* Camera3D UBO_Shared shared *//
layout (std140, binding = 0) uniform GPU_UBO_buffer_shared
{
	mat4 ViewProjection;
};

void main()
{
	InstanceData instance = instances[gl_InstanceID];
	mat4 model = instance.Model;

	// PSX-style wavy vertex effect
	vec3 wavyPos = aPos;
	wavyPos.y += 0.1 * sin(4.0 * wavyPos.x + uTime * 2.0);
	wavyPos.x += 0.1 * sin(4.0 * wavyPos.z + uTime * 1.5);

	gl_Position = ViewProjection * model * vec4(wavyPos, 1.0);
	TexCoord = aTexCoord;
	Color = instance.Color * aColor;
	FragPos = vec3(model * vec4(wavyPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
}
