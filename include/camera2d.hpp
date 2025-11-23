#pragma once

#include "pch.hpp"

namespace graphics
{
	class Camera2D
	{
	public:
		Camera2D(unsigned int width, unsigned int height);
		Camera2D(const Camera2D &) = delete;
		~Camera2D() = default;

		glm::vec2 position;
		float zoom;
		unsigned int width;
		unsigned int height;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;

		void setPosition(const glm::vec2 &position);
		void move(const glm::vec2& delta);
		void setZoom(float zoom);

		void updateViewProjection();
		void updateView();
		void updateProjection();
	};
}
