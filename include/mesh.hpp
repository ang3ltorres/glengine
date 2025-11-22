#pragma once

#include "pch.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace graphics
{
	struct Vertex3D
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	class Mesh
	{
	public:
		struct alignas(16) GPU_SSBO
		{
			glm::mat4 Model;
			glm::vec4 Color; // Tint
		};

		static void initialize();
		static void finalize();

		static Shader *shader;
		static GLuint UBO_Shared; // ViewProjection

		Mesh(const char *objFile, Texture *texture, unsigned int maxInstances);
		Mesh(Vertex3D *vertices, unsigned int numVertices, unsigned int *indices, unsigned int numIndices, Texture *texture, unsigned int maxInstances);
		Mesh(const Mesh&) = delete;
		~Mesh();

		Texture *texture;
		GLuint VAO, VBO, EBO;
		GLuint SSBO;
		GPU_SSBO *SSBO_Data;

		unsigned int maxInstances;
		unsigned int currentInstance;
		unsigned int indexCount;

		void draw();
	};
}
