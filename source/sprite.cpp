#include "sprite.hpp"

using namespace gui;
using namespace glm;

Sprite::Sprite(Texture *texture, ivec4 src, ivec4 dst)
: Drawable::Drawable(texture)
{
	this->src = src;
	this->dst = dst;
}

Sprite::Sprite(Texture *texture, ivec4 dst)
: Drawable::Drawable(texture)
{
	this->src = {0, 0, texture->width, texture->height};
	this->dst = dst;
}

Sprite::Sprite(Texture *texture)
: Drawable::Drawable(texture)
{
	this->src = {0, 0, texture->width, texture->height};
	this->dst = {0, 0, texture->width, texture->height};
}

