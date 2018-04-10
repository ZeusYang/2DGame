#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <queue>
#include <list>

struct Object {
	glm::ivec2 Index;//数组下标
	glm::vec3 Color;//颜色
	Object(int r,int c) :Index(r, c) {
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

//算法逻辑类
class Algorithm{
public:
	Algorithm(GLuint x,GLuint y);
	//随机产生新的食物
	glm::ivec2 NewFood();
	//重置board的情况
	void ResetBoard(std::list<Object> &psnake, Object &pfood,
		std::vector<std::vector<int> > &pboard);
	//初始化蛇和食物
	void ResetSnakeAndFood();
	//广度优先遍历整个board的情况
	bool RefreshBoard(std::list<Object> &psnake, Object &pfood, 
		std::vector<std::vector<int> > &pboard);
	//找到一条安全的路径
	glm::ivec2 FindSafeWay();
	//随便找一条路
	glm::ivec2 AnyPossibleWay();
	//AI思考
	glm::ivec2 AIThinking();
	void Display();
	//移动蛇身
	bool make_move(glm::ivec2 step);
	std::list<Object> snake;//蛇
	std::shared_ptr<Object> food;//食物
	
private:
	//行数、列数
	GLuint row, col;
	//用来标记board中每个位置的状况，0是空的，1是蛇身，2是食物
	std::vector<std::vector<int> >board;
	int EMPTY, SNAKE, FOOD;
	//边界判断
	inline bool CouldMove(glm::ivec2 &target) {
		if (target.x < 0 || target.x >= row)return false;
		if (target.y < 0 || target.y >= col)return false;
		return true;
	}
	//二维数组的结点向上、下、左、右四个扩展方向
	const int dir[4][2] = {
		{ -1,0 },{ +1,0 },{ 0,-1 },{ 0,+1 }
	};
	//找到一条最短的路径的方向
	inline glm::ivec2 ShortestMove(std::list<Object> &psnake, 
		std::vector<std::vector<int> >pboard){
		int minv = SNAKE;
		glm::ivec2 move;
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 tmp = glm::ivec2(psnake.front().Index.x + dir[x][0], psnake.front().Index.y + dir[x][1]);
			if (CouldMove(tmp) && minv > pboard[tmp.x][tmp.y]) {
				minv = pboard[tmp.x][tmp.y];
				move = tmp;
			}
		}
		return move;
	}
};

