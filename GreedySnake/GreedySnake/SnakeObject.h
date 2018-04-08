#pragma once
#include <list>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "CollisionDetect.h"

struct Body {//蛇身
	glm::vec2 pos;
	glm::vec2 velocity;//记录每段蛇身的方向
	glm::vec3 Color;//每段蛇身的颜色
	Body(GLfloat r, GLfloat c) :pos(glm::vec2(r,c)) {
		int decision = rand() % 4;
		switch (decision) {
		case 0:Color = glm::vec3(0.2f, 0.6f, 1.0f); break;
		case 1:Color = glm::vec3(0.0f, 0.7f, 0.0f); break;
		case 2:Color = glm::vec3(0.8f, 0.8f, 0.4f); break;
		case 3:Color = glm::vec3(1.0f, 0.5f, 0.0f); break;
		default:
			Color = glm::vec3(1.0f, 0.5f, 0.0f); break;
		}
	}
};
const int totalFrame = 9;
const float perFrame = 0.125;

class SnakeObject :
	public GameObject
{
public:
	//下一个方向
	glm::vec2 nextdir;
	SnakeObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
	~SnakeObject();

	//添加蛇身
	void AddBody(glm::vec2 pos);
	//移动蛇身
	void Move(GLfloat dt);
	//重置
	void Reset(glm::vec2 position, glm::vec2 velocity);
	//渲染
	void Draw(SpriteRenderer &renderer);
	//获取蛇头位置
	glm::vec2 GetHeadPos();
	//自己吃自己
	bool isCollisionSelf();
private:
	std::list<Body> snake;
	int frameCounter;
	float test;
};

