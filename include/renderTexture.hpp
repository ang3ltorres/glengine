#pragma once

#include "pch.hpp"

#include "sprite.hpp"
#include "camera.hpp"

namespace gui
{
	class RenderTexture : public Sprite
	{
	public:
		RenderTexture(unsigned int width, unsigned int height, Camera *camera = nullptr);
		RenderTexture(const RenderTexture&) = delete;
		~RenderTexture();

		GLuint FBO;

		bool internalCamera;
		Camera *camera;
	};
}
