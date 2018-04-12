#pragma once
#include <vector>
#include <stack>
#include <glm/glm.hpp>

enum Neighbor { LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3 };

struct Cell {//迷宫单元
	int neighbors[4];//四个方向的邻居
	bool visited;//记录是否访问过了
	Cell() :visited(false) {
		neighbors[LEFT] = neighbors[UP] = neighbors[RIGHT] = neighbors[DOWN] = 0;
	}
};

struct Wall {//墙
	glm::vec2 Pos;//UI坐标
	float angle;//旋转角度
	Wall(glm::vec2 pos, float ang) :Pos(pos), angle(ang) {}
};

class MazeAlgorithm{
public:
	MazeAlgorithm(int r,int c, int ux, int uy);

	void Init();//初始化

	void GetWallFromCells();//从迷宫单元矩阵中找到全部墙然后放到walls中

	//迷宫生成算法
	//算法一：深度优先算法
	bool Generator_Dfs();

	std::vector<Wall> walls;//迷宫的墙

private:
	const int row, col;//迷宫单元的行、列数
	std::vector<std::vector<Cell>> cells;//迷宫单元矩阵

	//迷宫单元的大小
	const int unitX, unitY;
	
	//dfs方法生成迷宫的栈
	std::stack<glm::ivec2> record;
	//dfs方法生成迷宫的当前标记的位置
	glm::ivec2 cur;
	//每一帧dfs探索的次数
	int frame;

	//将二维数组转换成UI坐标，注意顺序是相反的
	inline glm::vec2 Index(const glm::ivec2 &index) {
		/*if(index.y == 0)
			return glm::vec2(index.y*unitX + unitX*0.5, index.x*unitY + unitX*0.5);
		else */
			return glm::vec2(index.y*unitX + unitX*0.5, index.x*unitY + unitX*0.5);
	}

	//迷宫单元四个方向的UI坐标
	std::pair<glm::vec2, float> dir[4];
	//二维数组的结点向上、下、左、右四个扩展方向
	const int to[4][2] = {
		{ 0,-1 },{ -1,0 },{ 0,+1 },{ +1,0 }
	};

	//边界检查
	inline bool CouldMove(const glm::ivec2 &target) {
		if (target.x < 0 || target.x >= row)return false;
		if (target.y < 0 || target.y >= col)return false;
		return true;
	}
};

