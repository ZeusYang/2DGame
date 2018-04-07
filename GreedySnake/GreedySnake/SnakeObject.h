#pragma once
#include <list>
#include <glm/glm.hpp>
#include "GameObject.h"

struct Body {//蛇身
	glm::vec2 pos;
	glm::vec2 target;//记录每次的目标位置
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

class SnakeObject :
	public GameObject
{
public:
	SnakeObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
	~SnakeObject();

	//添加蛇身
	void AddBody(glm::vec2 pos);
	//移动蛇身
	void Move(GLfloat dt);
	//重置
	void Reset(glm::vec2 position, glm::vec2 velocity);
	//改变蛇头方向
	void ChangeHeadDir();
	//渲染
	void Draw(SpriteRenderer &renderer);
private:
	std::list<Body> snake;
	glm::vec2 headdir;
};

