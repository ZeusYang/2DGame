#pragma once
#include "GameObject.h"

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
	// 道具类型
	std::string Type;
	GLfloat     Duration;
	GLboolean   Activated;
	// 构造函数
	PowerUp(std::string type, glm::vec3 color, GLfloat duration,
		glm::vec2 position, Texture2D texture)
		: GameObject(position, SIZE, texture, color, VELOCITY),
		Type(type), Duration(duration), Activated()
	{ }
};

