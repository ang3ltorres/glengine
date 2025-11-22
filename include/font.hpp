#pragma once

#include "pch.hpp"

#include "sprite.hpp"
#include "glyph.hpp"

namespace graphics
{
	class Font : public Sprite
	{
	public:
		Font(const char *fontPath, unsigned int fontSize);
		Font(const Font &) = delete;
		~Font();

		Glyph glyphs[96]; // Printable characters (32 - 127)
		unsigned int fontSize;
	};
}

