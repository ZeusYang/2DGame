#include "SnakeObject.h"
#include <iostream>

SnakeObject::SnakeObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f),
		velocity),frameCounter(1),nextdir(this->Velocity)
{
	snake.push_back(Body(pos.x, pos.y));
	snake.front().velocity = this->Velocity;
	snake.front().Color = glm::vec3(1.0f);
	AddBody(pos);
	AddBody(pos);
}

SnakeObject::~SnakeObject()
{
}

//添加蛇身
void SnakeObject::AddBody(glm::vec2 pos, bool way) {
	if(!way)pos = snake.front().pos + this->Velocity;
	Body newhead = Body(pos.x, pos.y);
	//头部颜色保持不变
	glm::vec3 tmpCol = newhead.Color;
	newhead.Color = snake.front().Color;
	snake.front().Color = tmpCol;

	snake.push_front(newhead);
	this->Position = pos;
	snake.front().velocity = this->Velocity;
}

//移动蛇身，玩家模式
void SnakeObject::Move(GLfloat dt, Algorithm &algorithm) {
	//因为要移动蛇身，之前棋盘的记录全部要重置
	algorithm.Reset();
	//移动身体
	++frameCounter;
	//移动每段蛇身
	for (std::list<Body>::iterator it = snake.begin(); it != snake.end(); ++it) {
		it->pos += it->velocity*perFrame;
		algorithm.Place(it->pos.x / this->Size.x, it->pos.y / this->Size.y);
	}
	this->Position = snake.front().pos;
	if (frameCounter == totalFrame) {
		frameCounter = 1;
		//更新每段蛇身的速度方向，前一段蛇身的前一次速度赋给下一段蛇身的当前速度
		glm::vec2 prevVel = this->Velocity;
		std::list<Body>::iterator it = snake.begin();
		it->velocity = nextdir;
		this->Velocity = nextdir;//记录蛇头的速度
		for (++it; it != snake.end(); ++it) {
			glm::vec2 tmp = it->velocity;
			it->velocity = prevVel;
			prevVel = tmp;
		}
	}
}

void SnakeObject::MoveByAi(glm::vec2 step, Algorithm &algorithm) {
	algorithm.Reset();
	glm::vec2 tmp = step;
	for (std::list<Body>::iterator it = snake.begin(); it != snake.end(); ++it) {
		glm::vec2 prev = it->pos;
		it->pos = step;
		step = prev;
		algorithm.Place(it->pos.x / this->Size.x, it->pos.y / this->Size.y);
	}
	this->Position = snake.front().pos;
}

void SnakeObject::DrawHead(SpriteRenderer &renderer, Texture2D  &sprite) {
	renderer.DrawSprite(sprite, snake.front().pos, this->Size, this->Rotation, snake.front().Color);
}

void SnakeObject::Draw(SpriteRenderer &renderer) {
	auto it = snake.begin();
	for (++it; it != snake.end(); ++it) {
		renderer.DrawSprite(this->Sprite, it->pos, this->Size, this->Rotation, it->Color);
	}
}

//重置
void SnakeObject::Reset(glm::vec2 position, glm::vec2 velocity, Algorithm &algorithm) {
	snake.clear();
	this->Position = position;
	this->Velocity = velocity;
	this->nextdir = velocity;
	snake.push_back(Body(position.x, position.y));
	snake.front().Color = glm::vec3(1.0f);
	snake.front().velocity = this->Velocity;
	AddBody(position, false);
	AddBody(position, false);
	frameCounter = 1;
	for (auto it = snake.begin(); it != snake.end(); ++it) {
		algorithm.Place(it->pos.x / this->Size.x, it->pos.y / this->Size.y);
	}
}

glm::vec2 SnakeObject::GetHeadPos() {//获取蛇头位置
	return snake.front().pos;
}

bool SnakeObject::isCollisionSelf(){//是否吃到自己
	std::list<Body>::iterator it = snake.begin();
	int counter = 1;
	for (++it; it != snake.end(); ++it,++counter) {
		if (CollisionDetect::CheckCollision2(snake.front().pos, this->Size, it->pos, this->Size)){
			std::cout << "collision->" << counter << " Length->" << snake.size() << std::endl;
			return GL_TRUE;
		}
	}
	return GL_FALSE;
}