#pragma once

#include "pch.hpp"

#include "sprite.hpp"
#include "camera2d.hpp"

namespace graphics
{
	class RenderTexture : public Sprite
	{
	public:
		RenderTexture(unsigned int width, unsigned int height, Camera2D *camera = nullptr);
		RenderTexture(const RenderTexture&) = delete;
		~RenderTexture();

		GLuint FBO;

		bool internalCamera;
		Camera2D *camera;
	};
}
