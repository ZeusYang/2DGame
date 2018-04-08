#include "Algorithm.h"



Algorithm::Algorithm(GLuint x, GLuint y)
	:gridX(x),gridY(y)
{
	board.resize(gridX, std::vector<int>(gridY, 0));
}


Algorithm::~Algorithm()
{
}
