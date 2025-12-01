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

		struct LIGHT
		{
			glm::vec4 position; // x=position, y=position, z=position, w=unused
			glm::vec4 color; // x=red, y=green, z=blue, w=unused
			glm::vec4 attenuation; // x=constant, y=linear, z=quadratic, w=unused
		};

		struct MATERIAL
		{
			glm::vec3 specular;
			float shininess;
		};

		struct alignas(16) GPU_SSBO
		{
			glm::mat4 Model;
			glm::vec4 Color; // Tint
		};

		struct alignas(16) GPU_UBO_CAMERA
		{
			glm::mat4 View;
			glm::mat4 Projection;
			glm::mat4 ViewProjection;
			glm::vec4 CameraPosition;
		};

		struct alignas(16) GPU_UBO_LIGHT
		{
			LIGHT Light[8];
			MATERIAL Material;
			int LightCount;
			int Enable;
		};

		static void initialize();
		static void finalize();

		static void toggleLighting(bool enable);
		static int addLight(glm::vec3 position, glm::vec3 color, glm::vec3 attenuation);
		static void removeLight(int index);
		
		static Shader *shader;

		static GLuint UBO_Shared_Camera; // ViewProjection
		
		static GLuint UBO_Shared_Light; // Light
		static GPU_UBO_LIGHT UBO_Light_Data;
		
		static GLuint defaultTexture;

		Mesh(const char *file, Texture *texture, unsigned int maxInstances);
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
		
		bool internalTexture;
	};
}
