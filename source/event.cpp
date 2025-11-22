#include "event.hpp"

#include "graphics.hpp"

using namespace graphics;

std::bitset<512> Event::keyboardStates;
std::bitset<8>   Event::mouseStates;
std::function<void()> Event::updateCallback = nullptr;
std::function<void()> Event::mouseButtonCallback = nullptr;
std::function<void()> Event::cursorMovedCallback = nullptr;
glm::ivec2 Event::cursor;

void Event::EVENT_RESIZED([[maybe_unused]] GLFWwindow *window, int width, int height)
{
	Graphics::width  = width;
	Graphics::height = height;

	Graphics::defaultCamera->width  = width;
	Graphics::defaultCamera->height = height;

	Graphics::setRenderTexture();

	if (updateCallback != nullptr) updateCallback();
}

void Event::EVENT_KEYBOARD([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
	if (action == GLFW_PRESS)
		keyboardStates[key] = true;

	else if (action == GLFW_RELEASE)
		keyboardStates[key] = false;

	// if (updateCallback != nullptr) updateCallback();
}

void Event::EVENT_MOUSE_BUTTON(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
{
	if (action == GLFW_PRESS)
		mouseStates[button] = true;

	else if (action == GLFW_RELEASE)
		mouseStates[button] = false;

	// if (updateCallback != nullptr) updateCallback();

	if (action == GLFW_PRESS or action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Event::cursor = {xpos, ypos};
		
		if (mouseButtonCallback != nullptr) mouseButtonCallback();
	}
}

void Event::EVENT_CURSOR_MOVED([[maybe_unused]] GLFWwindow* window, double xpos, double ypos)
{
	Event::cursor = {xpos, ypos};
	if (cursorMovedCallback != nullptr) cursorMovedCallback();
}

