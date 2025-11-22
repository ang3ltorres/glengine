#include "font.hpp"

using namespace gui;

Font::Font(const char *fontPath, unsigned int fontSize)
: Drawable::Drawable( new Texture{fontPath, fontSize, glyphs, 128} ), fontSize(fontSize)
{}

Font::~Font()
{
	delete texture;
}
