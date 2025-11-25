#pragma once

#include "pch.hpp"
#include "texture.hpp"

namespace graphics
{
	struct Vertex3D
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec4 color;
	};

	class Mesh
	{
	public:
		struct alignas(16) GPU_SSBO
		{
		glm::mat4 Model;
		glm::vec4 Color; // Tint
		};

		// Phong lighting support
		struct PhongLight
		{
			glm::vec3 position;
			glm::vec3 color;
		};

		struct PhongMaterial
		{
			float shininess;
			glm::vec3 specular;
		};

		static void initialize();
		static void finalize();

		static Shader *shader;
		static Shader *phongShader;
		static GLuint UBO_Shared; // ViewProjection
		
		static GLuint defaultTexture;

		// Phong lighting static storage
		static PhongLight pLight;
		static PhongMaterial pMaterial;
		static glm::vec3 pViewPos;
		static bool pEnable;

		Mesh(const char *file, Texture *texture, unsigned int maxInstances);
		Mesh(Vertex3D *vertices, unsigned int numVertices, unsigned int *indices, unsigned int numIndices, Texture *texture, unsigned int maxInstances);
		Mesh(const Mesh&) = delete;
		~Mesh();

		Texture *texture;
		bool ownsTexture = false;
		GLuint VAO, VBO, EBO;
		GLuint SSBO;
		GPU_SSBO *SSBO_Data;

		unsigned int maxInstances;
		unsigned int currentInstance;
		unsigned int indexCount;
	};
}
