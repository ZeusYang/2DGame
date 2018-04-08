#pragma once
#include "TextRenderer.h"
#include <string>

class Menu
{
public:

	static void Selection(TextRenderer &text, GLuint &width, GLuint &height);

	static void Win(TextRenderer &text, GLuint &width, GLuint &height);

	static void Lost(TextRenderer &text, GLuint &width, GLuint &height, std::string &score);

private:
	Menu() {}
};

