#pragma once
#include "GameObject.h"

class BallObject : public GameObject
{
public:
	// 球的状态 
	GLfloat   Radius;
	GLboolean Stuck;

	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

	//根据球的速度移动球
	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void      Reset(glm::vec2 position, glm::vec2 velocity);
};

