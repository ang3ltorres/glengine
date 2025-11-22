#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

//* SSBO *//
struct GPU_SSBO
{
	vec4 src;
	vec4 tint;
	mat4 model;
};

layout(std430, binding = 1) buffer GPU_SSBO_buffer
{
	GPU_SSBO common_data[];
};

//* Camera UBO_Shared shared *//
layout(std140, binding = 0) uniform GPU_UBO_buffer_shared
{
	mat4 ViewProjection;
};

out vec4 tint;
out vec2 TexCoord;

void main()
{
	GPU_SSBO common_data =  common_data[gl_InstanceID];

	TexCoord    = common_data.src.xy + aTexCoord * common_data.src.zw;
	gl_Position = ViewProjection * common_data.model * vec4(aPos, 0.0, 1.0);
	
	tint = common_data.tint / 255.0;
}
