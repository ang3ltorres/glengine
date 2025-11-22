#version 460 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D image;

void main()
{
	vec4 texColor = texture(image, TexCoord);
	if(texColor.a < 0.1)
		discard;
		
	FragColor = texColor * Color;
}
