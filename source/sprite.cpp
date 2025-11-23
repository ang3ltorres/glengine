#include "sprite.hpp"

#include "graphics.hpp"

using namespace graphics;
using namespace glm;

Sprite::Sprite(Texture *texture, ivec4 src, ivec4 dst)
: texture(texture), src({0, 0, texture->width, texture->height}), dst({0, 0, texture->width, texture->height}), color({255, 255, 255, 255}), rotation(0.0f)
{
	this->src = src;
	this->dst = dst;

	updateModel();
}

Sprite::Sprite(Texture *texture, ivec4 dst)
: texture(texture), src({0, 0, texture->width, texture->height}), dst({0, 0, texture->width, texture->height}), color({255, 255, 255, 255}), rotation(0.0f)
{
	this->src = {0, 0, texture->width, texture->height};
	this->dst = dst;

	updateModel();
}

Sprite::Sprite(Texture *texture)
: texture(texture), src({0, 0, texture->width, texture->height}), dst({0, 0, texture->width, texture->height}), color({255, 255, 255, 255}), rotation(0.0f)
{
	this->src = {0, 0, texture->width, texture->height};
	this->dst = {0, 0, texture->width, texture->height};

	updateModel();
}

void Sprite::updateModel()
{
	model =
	{
		translate(mat4(1.0f), vec3(dst.x, dst.y, 0.0f))
			*
		rotate(mat4(1.0f), radians(rotation), {0.0f, 0.0f, 1.0f})
			*
		scale(mat4(1.0f), vec3(dst.z, dst.w, 1.0f))
	};
}

void Sprite::batch()
{
	texture->SSBO_Data[texture->currentInstance++] =
	{
		{(float)src.x / (float)texture->width, (float)src.y / (float)texture->height, (float)src.z / (float)texture->width, (float)src.w / (float)texture->height},
		{vec4{color.r, color.g, color.b, color.a}},
		{model},
	};
}

void Sprite::draw()
{
	Texture::shader->use();
	Graphics::setVAO(Texture::VAO);
	Graphics::setTexture(texture->id);
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, texture->SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Texture::GPU_SSBO) * texture->currentInstance, texture->SSBO_Data);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Texture::UBO_Shared);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, texture->UBO_NonShared);
	
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, texture->currentInstance);
	texture->currentInstance = 0;
}
