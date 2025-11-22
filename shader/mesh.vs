#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec4 Color;

struct InstanceData
{
	mat4 Model;
	vec4 Color;
};

layout (std430, binding = 1) buffer SSBO_Data
{
	InstanceData instances[];
};

layout (std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
};

void main()
{
	InstanceData instance = instances[gl_InstanceID];

	gl_Position = ViewProjection * instance.Model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	Color = instance.Color;
}
