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
	//music->play();

	Texture *txr = new Texture("../res/png_test.png", 1);
	Sprite *spr = new Sprite(txr);
	Mesh *mesh = new Mesh("../res/stage01.glb", nullptr, 1);
	Model *model = new Model(mesh);
	Font *font = new Font("../res/mononoki.ttf", 64);
	Text *text = new Text(font, "OpenGL 3D Test");
	text->render("Hello !");

	// Setup Camera
	Graphics::currentCamera3D->position = {7.0f, 2.0f, 0.0f};
	glm::vec3 dir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - Graphics::currentCamera3D->position);
	Graphics::currentCamera3D->yaw = glm::degrees(atan2(dir.z, dir.x));
	Graphics::currentCamera3D->pitch = glm::degrees(asin(dir.y));
	Graphics::currentCamera3D->fov = 70.0f;
	Graphics::setCamera3D(Graphics::currentCamera3D);

	model->scale = {1.0f, 1.0f, 1.0f};
	model->updateModel();

	// Setup Lighting
	Mesh::toggleLighting(true);
	Mesh::addLight({7.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.04f, 0.016f});
	Mesh::addLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.09f, 0.032f});

	while (!Graphics::shouldClose())
	{
		if (Event::keyboardStates[GLFW_KEY_ESCAPE])
			Graphics::forceClose = true;

		if (Event::keyboardStates[GLFW_KEY_RIGHT])
		{
			music->setPitch(pitch);	
			pitch += 0.001f;
		}

		if (Event::keyboardStates[GLFW_KEY_LEFT])
		{
			music->setPitch(pitch);
			pitch -= 0.001f;
		}

		Graphics::clearScreen({0, 0, 0, 255});

		Graphics::set2D();
		spr->batch();
		spr->draw();

		text->renderTexture->batch();
		text->renderTexture->draw();

		// Rotate model
		model->rotation.y += 0.3f;
		//model->rotation.x += 0.3f;
		//model->rotation.z -= 0.2f;
		model->updateModel();

		Graphics::set3D();
		model->batch();
		model->draw();

		Graphics::drawBuffer();
		Graphics::pollEvents();
	}

	delete text;
	delete font;
	delete model;
	delete mesh;
	delete txr;

	Audio::finalize();
	Graphics::finalize();
	return 0;
}