#include "graphics.hpp"
#include "audio.hpp"

using namespace graphics;
using namespace audio;

int main()
{
	unsigned int width = 1280;
	unsigned int height = 720;

	Graphics::initialize(width, height, "OpenGL 3D Test");
	
	Audio::initialize();
	Sound *music = new Sound("../res/music.ogg");
	float pitch = 1.0f;
	music->setPitch(pitch);
	music->setLooping(true);
	music->setVolume(1.0);
	music->play();

	// Define a cube
	// Define a cube with separate vertices for each face to allow unique UVs
	Vertex3D vertices[] = {
		// Front face
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		// Back face
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		// Left face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		// Right face
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		// Top face
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		// Bottom face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
	};

	unsigned int indices[] = {
		// Front
		0, 1, 2, 2, 3, 0,
		// Back
		4, 5, 6, 6, 7, 4,
		// Left
		8, 9, 10, 10, 11, 8,
		// Right
		12, 13, 14, 14, 15, 12,
		// Top
		16, 17, 18, 18, 19, 16,
		// Bottom
		20, 21, 22, 22, 23, 20
	};

	Texture *txr = new Texture("../res/png_test.png", 1);
	Texture *txr_cube = new Texture("../res/rect1.png", 1);
	Sprite *spr = new Sprite(txr);
	Mesh *mesh2 = new Mesh(vertices, 24, indices, 36, txr, 1);
	Mesh *mesh = new Mesh("../res/cubeColor.glb", nullptr, 1);
	Model *model = new Model(mesh);

	// Setup Camera
	Graphics::currentCamera3D->position = {-4.0f, 00.0f, 0.0f};
	Graphics::currentCamera3D->fov = 80.0f;
	Graphics::setCamera3D(Graphics::currentCamera3D);

	model->scale = {1.0f, 1.0f, 1.0f};

	while (!Graphics::shouldClose())
	{
		if (Event::keyboardStates[GLFW_KEY_ESCAPE])
			Graphics::forceClose = true;

		if (Event::keyboardStates[GLFW_KEY_RIGHT]) {
			music->setPitch(pitch);	
			pitch += 0.001f;
		}

		if (Event::keyboardStates[GLFW_KEY_LEFT]) {
			music->setPitch(pitch);
			pitch -= 0.001f;
		}

		Graphics::clearScreen({0, 0, 0, 255});

		Graphics::set2D();
		//spr->batch();
		//spr->draw();

		// Rotate model
		model->rotation.y += 0.6f;
		model->rotation.x += 0.3f;
		model->rotation.z -= 0.2f;
		model->updateModel();

		Graphics::set3D();
		model->batch();
		model->draw();

		Graphics::drawBuffer();
		Graphics::pollEvents();
	}

	delete model;
	delete mesh;
	delete mesh2;
	delete txr;
	delete txr_cube;

	Audio::finalize();
	Graphics::finalize();
	return 0;
}