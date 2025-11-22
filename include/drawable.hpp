#pragma once

#include "pch.hpp"

#include "texture.hpp"

namespace gui
{
	class Drawable
	{
	public:
		Drawable(Texture *texture);
		Drawable(const Drawable&) = delete;
		~Drawable() = default;

		Texture *texture;
		
		glm::ivec4 src;
		glm::ivec4 dst;
		glm::uvec4 color;
		float rotation;

		void updateModel();
		void batch();

		glm::mat4 model;
	};
}
