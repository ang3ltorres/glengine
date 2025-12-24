#include "texture.hpp"

#include "graphics.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace graphics;

//* Generic Vertex data for drawing 2d stuff
struct Vertex
{
	GLfloat position[2];
	GLfloat uv[2];
};

static const Vertex vertices[] =
{
	{ { 0.0f, 0.0f }, { 0.0f, 0.0f } },
	{ { 1.0f, 0.0f }, { 1.0f, 0.0f } },
	{ { 0.0f, 1.0f }, { 0.0f, 1.0f } },
	{ { 1.0f, 1.0f }, { 1.0f, 1.0f } }
};

static const unsigned int indices[] =
{
	0, 1, 2,
	1, 2, 3,
};

Shader *Texture::shader;

GLuint Texture::SSBO;
Texture::GPU_SSBO *Texture::SSBO_Data;
unsigned int Texture::maxInstances;
unsigned int Texture::currentInstance;

GLuint Texture::VAO;
GLuint Texture::VBO;
GLuint Texture::EBO;
GLuint Texture::UBO_Shared_Camera;

void Texture::initialize()
{
	//* VAO/Shader For 2D Texture drawing
	Texture::shader = new Shader("../shader/texture.vs", "../shader/texture.fs");

	Texture::maxInstances = 256;
	Texture::currentInstance = 0;
	glCreateBuffers(1, &Texture::SSBO);
	glNamedBufferData(Texture::SSBO, sizeof(GPU_SSBO) * Texture::maxInstances, nullptr, GL_STREAM_DRAW);
	Texture::SSBO_Data = new GPU_SSBO[Texture::maxInstances];

	glCreateVertexArrays(1, &Texture::VAO);

	glCreateBuffers(1, &Texture::VBO);
	glNamedBufferStorage(Texture::VBO, sizeof(vertices), vertices, 0);

	glCreateBuffers(1, &Texture::EBO);
	glNamedBufferStorage(Texture::EBO, sizeof(indices), indices, 0);

	glVertexArrayElementBuffer(Texture::VAO, Texture::EBO);

	GLuint vbufIndex = 0;
	glVertexArrayVertexBuffer(Texture::VAO, vbufIndex, Texture::VBO, 0, sizeof(Vertex));

	//! layout (location = 0) in vec2 aPos;
	GLuint aPos_location = 0;
	glVertexArrayAttribFormat(Texture::VAO, aPos_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(Texture::VAO, aPos_location, vbufIndex);
	glEnableVertexArrayAttrib(Texture::VAO, aPos_location);

	//! layout (location = 1) in vec2 aTexCoord;
	GLuint aTexCoord_location = 1;
	glVertexArrayAttribFormat(Texture::VAO, aTexCoord_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribBinding(Texture::VAO, aTexCoord_location, vbufIndex);
	glEnableVertexArrayAttrib(Texture::VAO, aTexCoord_location);

	glCreateBuffers(1, &Texture::UBO_Shared_Camera);
	glNamedBufferData(Texture::UBO_Shared_Camera, sizeof(Texture::GPU_UBO_CAMERA), &Graphics::currentCamera->viewProjection, GL_STREAM_DRAW);
	glClearNamedBufferData(Texture::UBO_Shared_Camera, GL_R32F, GL_RED, GL_FLOAT, nullptr); // memset to zero
}

void Texture::finalize()
{
	glDeleteBuffers(1, &Texture::UBO_Shared_Camera);
	glDeleteBuffers(1, &Texture::EBO);
	glDeleteBuffers(1, &Texture::VBO);
	glDeleteVertexArrays(1, &Texture::VAO);
	glDeleteBuffers(1, &Texture::SSBO);
	delete[] Texture::SSBO_Data;
	delete Texture::shader;
}

void Texture::getPixelDataPNG(const char *fileName, unsigned char *&buffer, unsigned int *width, unsigned int *height)
{
	int w = 0, h = 0, comp = 0;
	// Load image with 4 channels (RGBA)
	
	unsigned char *data = stbi_load(fileName, &w, &h, &comp, 4);

	*width = (unsigned int)w;
	*height = (unsigned int)h;
	size_t size = w * h * 4;
	buffer = new unsigned char[size];
	memcpy(buffer, data, size);
	stbi_image_free(data);
}

void Texture::getPixelDataFont(const char *fontPath, unsigned int fontSize, Glyph *glyphs, unsigned char *&buffer, unsigned int *width, unsigned int *height)
{
	FT_Library ft;
	FT_Init_FreeType(&ft);

	FT_Face face;
	FT_New_Face(ft, fontPath, 0, &face);
	
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// Determine texture size
	unsigned int maxGlyphWidth = 0, maxGlyphHeight = 0;
	unsigned int totalWidth = 0, currentRowHeight = 0;
	for (unsigned int c = 32; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		FT_Bitmap &bitmap = face->glyph->bitmap;
		maxGlyphWidth = std::max(maxGlyphWidth, bitmap.width);
		maxGlyphHeight = std::max(maxGlyphHeight, bitmap.rows);

		totalWidth += bitmap.width + 2; // Add padding between glyphs
		if (totalWidth > 1024) // New row if exceeds maximum width
		{
			totalWidth = bitmap.width + 2;
			currentRowHeight += maxGlyphHeight + 2;
		}
	}
	currentRowHeight += maxGlyphHeight; // Add final row height

	// Set texture size
	*width = 1024; // Fixed atlas width
	*height = currentRowHeight + 2; // Add padding for last row

	// Initialize texture buffer
	buffer = new unsigned char[(*width) * (*height) * 4];
	memset(buffer, 0, (*width) * (*height) * 4);

	unsigned int xOffset = 0, yOffset = 0, rowHeight = 0;

	for (unsigned int c = 32; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;

		FT_Bitmap &bitmap = face->glyph->bitmap;
		if (xOffset + bitmap.width > *width)
		{
			xOffset = 0;
			yOffset += rowHeight + 2;
			rowHeight = 0;
		}

		// Copy bitmap to texture buffer
		for (unsigned int y = 0; y < bitmap.rows; y++)
		{
			for (unsigned int x = 0; x < bitmap.width; x++)
			{
				unsigned int bufferIndex = ((yOffset + y) * (*width) + (xOffset + x)) * 4;
				unsigned char alpha = bitmap.buffer[y * bitmap.pitch + x];

				buffer[bufferIndex + 0] = 255;   // R
				buffer[bufferIndex + 1] = 255;   // G
				buffer[bufferIndex + 2] = 255;   // B
				buffer[bufferIndex + 3] = alpha; // A
			}
		}

		// Store glyph data
		glyphs[c - 32] =
		{
			glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			glm::vec2(bitmap.width, bitmap.rows),
			glm::ivec2(xOffset, yOffset),
			float(face->glyph->advance.x >> 6)
		};

		xOffset += bitmap.width + 2; // Move to next position
		rowHeight = std::max(rowHeight, bitmap.rows);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Texture::createTexture(const unsigned char *pixelData, bool free)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (free)
		delete[] pixelData;
}

void Texture::createBuffers(int textureType)
{
	Type = textureType;
	glCreateBuffers(1, &UBO_NonShared);
	glNamedBufferData(UBO_NonShared, sizeof(int), &Type, GL_STREAM_DRAW);
}

Texture::Texture(const char *fileName)
{
	unsigned char *pixelData;
	Texture::getPixelDataPNG(fileName, pixelData, &width, &height);
	createTexture(pixelData);
	createBuffers(0);
}

Texture::Texture(const char *fontPath, unsigned int fontSize, Glyph *glyphs)
{
	unsigned char *pixelData;
	Texture::getPixelDataFont(fontPath, fontSize, glyphs, pixelData, &width, &height);
	createTexture(pixelData);
	createBuffers(2);
}

Texture::Texture(unsigned int width, unsigned int height)
: width(width), height(height)
{
	unsigned char *pixelData = nullptr;
	createTexture(pixelData, false);
	createBuffers(1);
}

Texture::Texture(const unsigned char *pixelData, unsigned int width, unsigned int height)
: width(width), height(height)
{
	createTexture(pixelData, false);
	createBuffers(1);
}

Texture::~Texture()
{
	glDeleteBuffers(1, &UBO_NonShared);
	glDeleteTextures(1, &id);
}

void Texture::updateTexture(const unsigned char *pixelData, unsigned int newWidth, unsigned int newHeight)
{
	unsigned int oldWidth = width;
	unsigned int oldHeight = height;

	width  = newWidth;
	height = newHeight;

	if ((oldWidth != newWidth) or (oldHeight != newHeight))
	{
		GLuint newID;
		glCreateTextures(GL_TEXTURE_2D, 1, &newID);

		glTextureStorage2D(newID, 1, GL_RGBA8, width, height);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTextureSubImage2D(newID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

		glTextureParameteri(newID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(newID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(newID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(newID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLuint oldID = id;
		id = newID;

		glDeleteTextures(1, &oldID);
	}
	else
		glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
}
