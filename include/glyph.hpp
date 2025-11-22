#pragma once

#include "pch.hpp"

namespace gui
{
	struct Glyph
	{
		glm::vec2 bearing;   // Offset from baseline to top-left of glyph
		glm::vec2 size;      // Size of the glyph in pixels
		glm::ivec2 atlasPos; // Position in the texture atlas
		float advance;       // Horizontal offset to advance to next glyph
	};
}
