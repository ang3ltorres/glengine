#include "font.hpp"

using namespace gui;

Font::Font(const char *fontPath, unsigned int fontSize)
: Sprite::Sprite( new Texture{fontPath, fontSize, glyphs, 128} ), fontSize(fontSize)
{}

Font::~Font()
{
	delete texture;
}
