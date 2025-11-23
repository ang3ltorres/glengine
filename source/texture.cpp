#include "texture.hpp"

#include "graphics.hpp"

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
GLuint Texture::VAO;
GLuint Texture::VBO;
GLuint Texture::EBO;
GLuint Texture::UBO_Shared;

void Texture::initialize()
{
	//* VAO/Shader For 2D Texture drawing
	Texture::shader = new Shader("../shader/texture.vs", "../shader/texture.fs");

	glCreateVertexArrays(1, &Texture::VAO);

	glCreateBuffers(1, &Texture::VBO);
	glNamedBufferStorage(Texture::VBO, sizeof(vertices), vertices, 0);

	glCreateBuffers(1, &Texture::EBO);
	glNamedBufferStorage(Texture::EBO, sizeof(indices), indices, 0);

	glVertexArrayElementBuffer(Texture::VAO, Texture::EBO);

	GLuint vbufIndex = 0;
	glVertexArrayVertexBuffer(Texture::VAO, vbufIndex, Texture::VBO, 0, sizeof(Vertex));

	//! layout(location = 0) in vec2 aPos;
	GLuint aPos_location = 0;
	glVertexArrayAttribFormat(Texture::VAO, aPos_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(Texture::VAO, aPos_location, vbufIndex);
	glEnableVertexArrayAttrib(Texture::VAO, aPos_location);

	//! layout(location = 1) in vec2 aTexCoord;
	GLuint aTexCoord_location = 1;
	glVertexArrayAttribFormat(Texture::VAO, aTexCoord_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribBinding(Texture::VAO, aTexCoord_location, vbufIndex);
	glEnableVertexArrayAttrib(Texture::VAO, aTexCoord_location);

	//? Submit camera data to GPU (UBO_Shared)
	glCreateBuffers(1, &Texture::UBO_Shared);
	glNamedBufferData(Texture::UBO_Shared, sizeof(glm::mat4), &Graphics::currentCamera->viewProjection, GL_STREAM_DRAW);
}

void Texture::finalize()
{
	glDeleteBuffers(1, &Texture::UBO_Shared);
	glDeleteBuffers(1, &Texture::EBO);
	glDeleteBuffers(1, &Texture::VBO);
	glDeleteVertexArrays(1, &Texture::VAO);
	delete Texture::shader;
}

void Texture::getPixelDataPNG(const char *fileName, unsigned char *&buffer, unsigned int *width, unsigned int *height)
{
	FILE *fp = fopen(fileName, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info = png_create_info_struct(png);

	png_init_io(png, fp);
	png_read_info(png, info);

	*width  = png_get_image_width(png, info);
	*height = png_get_image_height(png, info);

	if (png_get_bit_depth(png, info) == 16)
		png_set_strip_16(png);

	if (png_get_color_type(png, info) == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	if (png_get_color_type(png, info) == PNG_COLOR_TYPE_GRAY || 
		png_get_color_type(png, info) == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	size_t row_bytes = png_get_rowbytes(png, info);

	buffer = new unsigned char[row_bytes * (*height)];

	png_bytep *row_pointers = new png_bytep[*height];
	for (unsigned int y = 0; y < *height; y++)
		row_pointers[y] = buffer + y * row_bytes;

	png_read_image(png, row_pointers);

	delete[] row_pointers;
	png_destroy_read_struct(&png, &info, nullptr);
	fclose(fp);
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
	glCreateBuffers(1, &SSBO);
	glNamedBufferData(SSBO, sizeof(GPU_SSBO) * maxInstances, nullptr, GL_STREAM_DRAW);
	SSBO_Data = new GPU_SSBO[maxInstances];

	Type = textureType;
	glCreateBuffers(1, &UBO_NonShared);
	glNamedBufferData(UBO_NonShared, sizeof(int), &Type, GL_STREAM_DRAW);
}

Texture::Texture(const char *fileName, unsigned int maxInstances)
: maxInstances(maxInstances), currentInstance(0)
{
	unsigned char *pixelData;
	Texture::getPixelDataPNG(fileName, pixelData, &width, &height);
	createTexture(pixelData);
	createBuffers(0);
}

Texture::Texture(const char *fontPath, unsigned int fontSize, Glyph *glyphs, unsigned int maxInstances)
: maxInstances(maxInstances), currentInstance(0)
{
	unsigned char *pixelData;
	Texture::getPixelDataFont(fontPath, fontSize, glyphs, pixelData, &width, &height);
	createTexture(pixelData);
	createBuffers(2);
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int maxInstances)
: width(width), height(height), maxInstances(maxInstances), currentInstance(0)
{
	unsigned char *pixelData = nullptr;
	createTexture(pixelData, false);
	createBuffers(1);
}

Texture::Texture(const unsigned char *pixelData, unsigned int width, unsigned int height, unsigned int maxInstances)
: width(width), height(height), maxInstances(maxInstances), currentInstance(0)
{
	createTexture(pixelData, false);
	createBuffers(1);
}

Texture::~Texture()
{
	glDeleteBuffers(1, &UBO_NonShared);
	glDeleteBuffers(1, &SSBO);
	glDeleteTextures(1, &id);
	delete[] SSBO_Data;
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
