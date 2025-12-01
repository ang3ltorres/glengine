#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

struct InstanceData
{
	vec4 src;
	vec4 tint;
	mat4 model;
};

layout (std430, binding = 1) buffer SSBO_Data
{
	InstanceData instances[];
};

layout (std140, binding = 0) uniform UBO_Shared_Camera
{
	mat4 ViewProjection;
};

out vec4 tint;
out vec2 TexCoord;

void main()
{
	InstanceData instance = instances[gl_InstanceID];

	TexCoord    = instance.src.xy + aTexCoord * instance.src.zw;
	gl_Position = ViewProjection * instance.model * vec4(aPos, 0.0, 1.0);
	
	tint = instance.tint / 255.0;
}
