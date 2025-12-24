#include "renderTexture.hpp"

using namespace graphics;
using namespace glm;

RenderTexture::RenderTexture(unsigned int width, unsigned int height, Camera2D *camera)
: Sprite::Sprite(new Texture(width, height))
{
	// Camera2D
	internalCamera = camera ? false : true;
	this->camera = camera ? camera : new Camera2D(width, height);

	glCreateFramebuffers(1, &FBO);
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, texture->id, 0);
}

RenderTexture::~RenderTexture()
{
	if (internalCamera)
		delete camera;
		
	delete texture;
	glDeleteFramebuffers(1, &FBO);
}

