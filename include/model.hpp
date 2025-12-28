#pragma once

#include "pch.hpp"
#include "mesh.hpp"

namespace graphics
{
	class Model
	{
	public:
		Model(Mesh *mesh, Mesh::GPU_MATERIAL material = {glm::vec3(1.0f), 32.0f});
		Model(const Model&) = default;
		~Model() = default;

		Mesh *mesh;

		glm::vec3 position;
		glm::vec3 rotation; // Euler angles in degrees
		glm::vec3 scale;
		glm::vec4 color; // Tint color
		Mesh::GPU_MATERIAL material;

		glm::mat4 modelMatrix;

		void updateModel();
		void batch();
		void draw();
	};
}
