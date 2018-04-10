#include "SnakeObject.h"
#include <iostream>

SnakeObject::SnakeObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f),
		velocity)
{
}

SnakeObject::~SnakeObject()
{
}

void SnakeObject::DrawSnake(std::list<Object> &target, SpriteRenderer &renderer, Texture2D  &headSprite) {
	auto it = target.begin();
	this->Position = Index(it->Index);
	renderer.DrawSprite(headSprite, Index(it->Index), this->Size, this->Rotation, it->Color);//Í·²¿
	for (++it; it != target.end(); ++it) {
		renderer.DrawSprite(this->Sprite, Index(it->Index), this->Size, this->Rotation, it->Color);
	}
	
}


