#pragma once

#include "pch.hpp"

#include "renderTexture.hpp"
#include "font.hpp"

namespace gui
{
	class Text
	{
	public:
		Text(gui::Font *font, const char *text, float gap = 1.0f);
		Text(const Text &) = delete;
		~Text();

		gui::Font *font;
		gui::RenderTexture *renderTexture;

		void render(const char *text);
		float gap;
	};
}

