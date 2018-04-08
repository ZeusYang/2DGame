#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class Algorithm
{
public:
	Algorithm(GLuint x,GLuint y);
	~Algorithm();

private:
	GLuint gridX, gridY;
	std::vector<std::vector<int> >board;
};

