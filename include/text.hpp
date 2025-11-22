#pragma once

#include "pch.hpp"

#include "renderTexture.hpp"
#include "font.hpp"

namespace graphics
{
	class Text
	{
	public:
		Text(Font *font, const char *text, float gap = 1.0f);
		Text(const Text &) = delete;
		~Text();

		Font *font;
		RenderTexture *renderTexture;

		void render(const char *text);
		float gap;
	};
}

