#pragma once

#include "pch.hpp"

namespace graphics
{
	class Camera2D
	{
	public:
		glm::vec2 position;
		float zoom;

		//
		unsigned int width;
		unsigned int height;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;

		Camera2D(unsigned int width, unsigned int height);
		Camera2D(const Camera2D &) = delete;
		~Camera2D() = default;
		void setPosition(const glm::vec2 &position);
		void move(const glm::vec2& delta);
		void setZoom(float zoom);
		void updateViewProjectionMatrix();
		void updateView();
		void updateProjection();
	};
}
