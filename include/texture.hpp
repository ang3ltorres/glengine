#pragma once

#include "pch.hpp"

#include "shader.hpp"

namespace graphics
{
	struct Glyph
	{
		glm::vec2 bearing;   // Offset from baseline to top-left of glyph
		glm::vec2 size;      // Size of the glyph in pixels
		glm::ivec2 atlasPos; // Position in the texture atlas
		float advance;       // Horizontal offset to advance to next glyph
	};

	class Texture
	{
	public:

		struct alignas(16) GPU_SSBO
		{
			glm::vec4 Src;
			glm::vec4 TintColor;
			glm::mat4 Model;
		};

		struct alignas(16) GPU_UBO_CAMERA
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

		static GLuint SSBO;
		static GPU_SSBO *SSBO_Data;
		static unsigned int maxInstances;
		static unsigned int currentInstance;

		static GLuint VAO;
		static GLuint VBO;
		static GLuint EBO;
		static GLuint UBO_Shared_Camera;
		
		// Constructor common
		void createTexture(const unsigned char *pixelData, bool free = true);
		void createBuffers(int textureType);

		Texture(const char *fileName);
		Texture(const char *fontPath, unsigned int fontSize, Glyph *glyphs);
		Texture(unsigned int width, unsigned int height);
		Texture(const unsigned char *pixelData, unsigned int width, unsigned int height);
		Texture(const Texture&) = delete;
		~Texture();

		void updateTexture(const unsigned char *pixelData, unsigned int newWidth, unsigned int newHeight);

		GLuint UBO_NonShared;
		int Type;
	};
}
