#pragma once

#include "pch.hpp"

#include "event.hpp"
#include "shader.hpp"
#include "camera2d.hpp"
#include "texture.hpp"
#include "sprite.hpp"
#include "renderTexture.hpp"
#include "text.hpp"

namespace graphics
{
	class Graphics
	{
	public:
		Graphics() = delete;
		Graphics(const Graphics&) = delete;
		~Graphics() = default;

		static void initialize(int width, int height, const char *title);
		static void finalize();
		
		static void begin();

		// Window
		static GLFWwindow *window;
		static unsigned int width;
		static unsigned int height;
		static bool forceClose;

		// Default stuff
		static GLuint currentVAO;
		static GLuint currentTexture;
		static Camera2D *currentCamera;
		static Camera2D *defaultCamera;

		// Time Travel stuff
		static float fps;
		static float delta;

		// Library Methods
		static bool shouldClose();
		static void clearScreen(const glm::uvec4 &color);
		static void setRenderTexture(RenderTexture *renderTexture = nullptr);
		static void setVAO(GLuint VAO);
		static void setTexture(GLuint texture);
		static void drawBuffer();
		static void pollEvents();
	};
}