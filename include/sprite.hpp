#pragma once

#include "pch.hpp"

#include "texture.hpp"

namespace graphics
{
	class Sprite
	{
	public:
		Sprite(const Sprite&) = default;
		Sprite(Texture *texture, glm::ivec4 src, glm::ivec4 dst);
		Sprite(Texture *texture, glm::ivec4 dst);
		Sprite(Texture *texture);
		~Sprite() = default;

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
