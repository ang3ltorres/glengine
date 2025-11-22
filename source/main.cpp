#include "graphics.hpp"

using namespace gui;

int main()
{
	unsigned int width = 1280;
	unsigned int height = 720;

	Graphics::initialize(width, height, "OpenGL");

	Texture *txr = new Texture("../res/png_test.png", 1);
	Sprite *spr = new Sprite(txr);
	
	while (!Graphics::shouldClose())
	{
		if (Event::keyboardStates[GLFW_KEY_ESCAPE])
			Graphics::forceClose = true;

    Graphics::clearScreen({255, 0, 0, 255});

		spr->batch();
		spr->texture->draw();

    Graphics::drawBuffer();
		Graphics::pollEvents();
	}

	delete txr;
	delete spr;

	Graphics::finalize();
	return 0;
}