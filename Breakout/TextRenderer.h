#pragma once
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance;     // Horizontal offset to advance to next glyph
};

class TextRenderer
{
public:
	// Holds a list of pre-compiled Characters
	std::map<GLchar, Character> Characters;
	// Shader used for text rendering
	Shader TextShader;
	// Constructor
	TextRenderer(GLuint width, GLuint height);
	// Pre-compiles a list of characters from the given font
	void Load(std::string font, GLuint fontSize);
	// Renders a string of text using the precompiled list of characters
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
private:
	// Render state
	GLuint VAO, VBO;
};

