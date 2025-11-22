#include "graphics.hpp"

using namespace gui;

int main()
{
	unsigned int width = 1280;
	unsigned int height = 720;

	Graphics::initialize(width, height, "OpenGL");
	
	while (!Graphics::shouldClose())
	{
		if (Event::keyboardStates[GLFW_KEY_ESCAPE])
			Graphics::forceClose = true;

    Graphics::clearScreen({255, 0, 0, 255});
    Graphics::drawBuffer();
		Graphics::pollEvents();
	}

	Graphics::finalize();
	return 0;
}