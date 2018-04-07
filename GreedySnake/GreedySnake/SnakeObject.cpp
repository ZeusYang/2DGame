#include "SnakeObject.h"
#include <iostream>

SnakeObject::SnakeObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f),
		velocity),headdir(velocity)
{
	AddBody(pos);
}

SnakeObject::~SnakeObject()
{
}

//添加蛇身
void SnakeObject::AddBody(glm::vec2 pos) {
	snake.push_front(Body(pos.x, pos.y));
	snake.front().target = pos + this->Velocity;
	this->Position = pos;
}

//移动蛇身
void SnakeObject::Move(GLfloat dt) {
	//移动身体
	//计算移动的距离
	//更新蛇头的位置
	glm::vec2 delta = glm::vec2((this->Velocity.x)*dt, (this->Velocity.y)*dt);
	glm::vec2 prev = snake.front().pos;
	snake.front().pos += delta;
	std::list<Body>::iterator it = snake.begin();
	++it;
	for (; it != snake.end(); ++it) {
		glm::vec2 tmp = it->pos;
		it->pos = prev;
		prev = tmp;
	}
	//更新蛇头的目标位置
	if (glm::length(snake.front().target - snake.front().pos) < 5.0f) {
		std::cout << " ???" << std::endl;
		this->Velocity = headdir;
		glm::vec2 next = snake.front().target + this->Velocity;
		snake.front().pos = snake.front().target;
		snake.front().target = next;
	}
	std::cout << snake.front().target.x << "," << snake.front().target.y << std::endl;
	this->Position = snake.front().pos;
}

void SnakeObject::Draw(SpriteRenderer &renderer) {
	for (auto & body : snake) {
		renderer.DrawSprite(this->Sprite, body.pos, this->Size, this->Rotation, body.Color);
	}
}

//重置
void SnakeObject::Reset(glm::vec2 position, glm::vec2 velocity) {
	snake.clear();
	snake.push_back(Body(position.x, position.y));
	this->Position = position;
	this->Velocity = velocity;
}

void SnakeObject::ChangeHeadDir() {
	glm::vec2 tmp = this->Velocity;
	this->Velocity = headdir;
	headdir = tmp;
}