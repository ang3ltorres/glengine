#include "graphics.hpp"
#include <vector>

using namespace graphics;

int main()
{
	unsigned int width = 1280;
	unsigned int height = 720;

	Graphics::initialize(width, height, "OpenGL 3D Test");

	// Define a cube
	Vertex3D vertices[] = {
		// Front face
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		// Back face
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
	};

	unsigned int indices[] = {
		// Front
		0, 1, 2, 2, 3, 0,
		// Right
		1, 5, 6, 6, 2, 1,
		// Back
		7, 6, 5, 5, 4, 7,
		// Left
		4, 0, 3, 3, 7, 4,
		// Bottom
		4, 5, 1, 1, 0, 4,
		// Top
		3, 2, 6, 6, 7, 3
	};

	Texture *txr = new Texture("../res/png_test.png", 1);
	Mesh *mesh = new Mesh(vertices, 8, indices, 36, txr, 1);
	Model *model = new Model(mesh);

	// Setup Camera
	Graphics::currentCamera3D->position = {0.0f, 0.0f, 3.0f};
	Graphics::setCamera3D(Graphics::currentCamera3D);

	while (!Graphics::shouldClose())
	{
		if (Event::keyboardStates[GLFW_KEY_ESCAPE])
			Graphics::forceClose = true;

		Graphics::clearScreen({50, 50, 50, 255});

		// Rotate model
		model->rotation.y += 1.0f;
		model->rotation.x += 0.5f;
		model->updateModel();

		model->batch();
		mesh->draw();

		Graphics::drawBuffer();
		Graphics::pollEvents();
	}

	delete model;
	delete mesh;
	delete txr;

	Graphics::finalize();
	return 0;
}