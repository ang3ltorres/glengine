#pragma once

#include "pch.hpp"

#include "shader.hpp"
#include "glyph.hpp"

namespace graphics
{
	class Texture
	{
	public:

		struct alignas(16) GPU_SSBO
		{
			glm::vec4 Src;
			glm::vec4 TintColor;
			glm::mat4 Model;
		};

		struct alignas(16) GPU_UBO
		{
			alignas(16) glm::mat4 ViewProjection;
		};

		GLuint id;
		GLuint width;
		GLuint height;

		static void initialize();
		static void finalize();

		static void getPixelDataPNG(const char *fileName, unsigned char *&buffer, unsigned int *width, unsigned int *height);
		static void getPixelDataFont(const char *fontPath, unsigned int fontSize, Glyph *glyphs, unsigned char *&buffer, unsigned int *width, unsigned int *height);

		//* Generic Draw Texture 2D stuff
		static Shader *shader;
		static GLuint VAO;
		static GLuint VBO;
		static GLuint EBO;
		static GLuint UBO_Shared;
		
		// Constructor common
		void createTexture(unsigned char *pixelData);
		void createBuffers(int textureType);

		Texture(const char *fileName, unsigned int maxInstances);
		Texture(const char *fontPath, unsigned int fontSize, Glyph *glyphs, unsigned int maxInstances);
		Texture(unsigned int width, unsigned int height, unsigned int maxInstances);
		Texture(const Texture&) = delete;
		~Texture();

		void updateTexture(unsigned char *pixelData, unsigned int newWidth, unsigned int newHeight);

		GLuint UBO_NonShared;
		int Type;

		GLuint SSBO;
		GPU_SSBO *SSBO_Data;
		unsigned int maxInstances;
		unsigned int currentInstance;

		void draw();
	};
}
