#pragma once

#include "pch.hpp"

namespace graphics
{
	class Camera3D
	{
	public:
		Camera3D(float width, float height);
		Camera3D(const Camera3D&) = delete;
		~Camera3D() = default;

		glm::vec3 position;
		float yaw;
		float pitch;
		float roll;

		float fov;
		float aspect;
		float nearClip;
		float farClip;

		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;

		void move(const glm::vec3& pos);

		void rotate(float dYaw, float dPitch, float dRoll = 0.0f);

		void updateView();
		void updateProjection();
		void updateViewProjection();

		glm::vec3 forward() const;
		glm::vec3 right() const;
		glm::vec3 up() const;
	};
}