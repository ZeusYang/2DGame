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
	//重置
	void ResetBoard(const std::list<Object> &psnake, const Object &pfood,
		std::vector<std::vector<int> > &pboard);
	void ResetSnakeAndFood();

	//广度优先遍历整个board的情况
	bool RefreshBoard(const std::list<Object> &psnake, const Object &pfood, 
		std::vector<std::vector<int> > &pboard);

	glm::ivec2 FindSafeWay();//找到一条安全的路径
	
	glm::ivec2 AnyPossibleWay();//随便找一条路
	
	glm::ivec2 AIThinking();//AI思考
	void Display();
	
	bool make_move(glm::ivec2 step);//移动蛇身

	void VirtualMove();//虚拟探测性检测
	bool IsTailInside();//评测是否蛇尾和蛇头之间有路径
	glm::ivec2 FollowTail();//朝蛇尾方向走
	std::list<Object> snake;//蛇
	std::shared_ptr<Object> food;//食物
	bool win;
	
private:
	//行数、列数
	GLuint row, col;
	std::vector<std::vector<int> >board;//用来标记board中每个位置的状况，0是空的，1是蛇身，2是食物

	//虚拟记录贪吃蛇的情况
	std::vector<std::vector<int> >tmpboard;
	std::list<Object> tmpsnake;

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
	inline glm::ivec2 ShortestMove(glm::ivec2 target, 
		const std::vector<std::vector<int> > &pboard){
		int minv = SNAKE;
		glm::ivec2 move(-1,-1);
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 tmp = glm::ivec2(target.x + dir[x][0], target.y + dir[x][1]);
			if (CouldMove(tmp) && minv > pboard[tmp.x][tmp.y]) {
				minv = pboard[tmp.x][tmp.y];
				move = tmp;
			}
		}
		return move;
	}

	//找到一条最长的路径的方向
	inline glm::ivec2 LongestMove(glm::ivec2 target,
		const std::vector<std::vector<int> > &pboard) {
		int mxav = -1;
		glm::ivec2 move(-1, -1);
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 tmp = glm::ivec2(target.x + dir[x][0], target.y + dir[x][1]);
			if (CouldMove(tmp) && pboard[tmp.x][tmp.y] < EMPTY && mxav < pboard[tmp.x][tmp.y]) {
				mxav = pboard[tmp.x][tmp.y];
				move = tmp;
			}
		}
		return move;
	}
};

