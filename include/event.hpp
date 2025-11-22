#pragma once

#include "pch.hpp"

namespace gui
{
	struct Event
	{
		Event() = delete;
		Event(const Event&) = delete;
		~Event() = default;
	
		static std::bitset<512> keyboardStates;
		static std::bitset<8> mouseStates;
		static std::function<void()> updateCallback;
		static std::function<void()> mouseButtonCallback;
		static std::function<void()> cursorMovedCallback;
		static glm::ivec2 cursor;
		
		static void EVENT_RESIZED(GLFWwindow *window, int width, int height);
		static void EVENT_KEYBOARD(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void EVENT_MOUSE_BUTTON(GLFWwindow* window, int button, int action, int mods);
		static void EVENT_CURSOR_MOVED(GLFWwindow* window, double xpos, double ypos);
	};
}


