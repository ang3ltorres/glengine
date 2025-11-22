#include "renderTexture.hpp"

using namespace graphics;
using namespace glm;

RenderTexture::RenderTexture(unsigned int width, unsigned int height, Camera2D *camera)
: Sprite::Sprite(new Texture(width, height, 1))
{
	// Camera2D
	internalCamera = camera ? false : true;
	this->camera = camera ? camera : new Camera2D(width, height);

	glCreateFramebuffers(1, &FBO);
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, texture->id, 0);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glNamedFramebufferDrawBuffers(FBO, 1, drawBuffers);
}

RenderTexture::~RenderTexture()
{
	if (internalCamera) delete camera;
	delete texture;
	glDeleteFramebuffers(1, &FBO);
}

