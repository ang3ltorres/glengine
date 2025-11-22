#version 460 core

in vec4 tint;
in vec2 TexCoord;

//* Texture type UBO_NonShared buffer *//
layout(std140, binding = 1) uniform GPU_UBO_buffer_nonshared
{
	int Type;
};

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
	switch (Type)
	{
		case 0:
			FragColor = texture(u_Texture, TexCoord) * tint;
			break;

		case 1:
			FragColor = (texture(u_Texture, vec2(TexCoord.x, 1.0 - TexCoord.y))) * tint;
			break;

		case 2:
			float alpha = texture(u_Texture, TexCoord).a;
			FragColor = vec4(tint.rgb, alpha);
			break;
	}
}
