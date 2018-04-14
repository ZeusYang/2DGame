#pragma once
#include <vector>
#include <stack>
#include <list>
#include <queue>
#include <glm/glm.hpp>

enum Neighbor { LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3 };

struct Cell {//迷宫单元
	int neighbors[4];//四个方向的邻居
	int visited;//记录是否访问过了
	//以下用于寻路算法
	glm::ivec2 prev;//记录前驱
	//用于A星算法的open表、closed表
	bool inOpen, inClosed;
	//启发式函数fn = gn + hn
	//其中gn为起点到n的实际距离，hn为n到终点的哈密顿
	int gn, hn;
	Cell() :visited(0),inOpen(false),inClosed(false) {
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

	void Generation_Init();
	void Pathfinding_Init();

	void GetWallFromCells();//从迷宫单元矩阵中找到全部墙然后放到walls中
	void GetPathFromCells();//从迷宫单元矩阵中找到被遍历过的路径放到paths中

	//迷宫生成算法
	//算法一：深度优先算法
	bool Generator_Dfs();
	//算法二：Prim随机算法
	bool Generator_Prim();
	//算法三：递归分割算法
	bool Generator_Recursive();

	//迷宫寻路算法：
	//算法一：深度优先遍历
	bool Pathfinding_Dfs();
	//算法二：广度优先遍历
	bool Pathfinding_Bfs();
	//算法三：A星算法
	bool Pathfinding_Astar();

	//根据前驱倒退路径
	void GetSolution();
	//移动冒险家一步
	bool AdventurerMove();

	std::vector<Wall> walls;//迷宫的墙
	std::vector<glm::vec2> paths;//遍历的路
	std::vector<glm::vec2> solution;//最终的路径
	glm::vec2 start, end;//迷宫的起点和终点
	glm::vec2 adventurer;//迷宫冒险家的位置
	int adven_index;//冒险家在solution中的索引

private:
	const int row, col;//迷宫单元的行、列数
	static std::vector<std::vector<Cell>> cells;//迷宫单元矩阵
	struct Node {//A星算法的结点
		glm::ivec2 index;//cells数组的下标索引
		Node(int r, int c) :index((glm::ivec2(r, c))) {}
	};
	class Compare {//优先队列的比较仿函数对象
	public:
		bool operator()(const Node& n1, const Node& n2) {
			return cells[n1.index.x][n1.index.y].gn + cells[n1.index.x][n1.index.y].hn
				> cells[n2.index.x][n2.index.y].gn + cells[n2.index.x][n2.index.y].hn;
		}
	};

	//迷宫单元的大小
	const int unitX, unitY;
	
	//迷宫生成算法一数据结构:深度优先的栈
	std::stack<glm::ivec2> record;
	//迷宫生成算法二数据结构：随机删除的链表
	std::list<glm::ivec2> prim;
	//迷宫生成算法三数据结构：四叉树广度优先的队列
	std::queue<std::pair<glm::ivec2, glm::ivec2>> recursive;

	//迷宫寻路算法一数据结构：深度优先的栈
	std::stack<glm::ivec2> path_dfs;
	//迷宫寻路算法一数据结构：广度优先的队列
	std::queue<glm::ivec2> path_bfs;
	//迷宫寻路算法一数据结构：A星算法的优先队列
	std::priority_queue<Node,std::vector<Node>,Compare> path_astar;

	//生成迷宫的当前标记的位置
	glm::ivec2 cur;
	//每一帧迷宫生成进展的次数，用于演示动画
	int frame;

	//将二维数组转换成UI坐标，注意顺序是相反的
	inline glm::vec2 Index(const glm::ivec2 &index) {
		return glm::vec2(index.y*unitX + unitX*0.8, index.x*unitY + unitX*0.8);
	}

	//迷宫单元四个方向的UI坐标
	std::pair<glm::vec2, float> dir[4];
	//二维数组的结点向左、上、右、下四个扩展方向
	const int to[4][2] = {
		{ 0,-1 },{ -1,0 },{ 0,+1 },{ +1,0 }
	};

	//边界检查
	inline bool CouldMove(const glm::ivec2 &target) {
		if (target.x < 0 || target.x >= row)return false;
		if (target.y < 0 || target.y >= col)return false;
		return true;
	}

	//计算哈密顿距离
	inline int DirectLen(const glm::ivec2& n1, const glm::ivec2& n2) {
		return abs(n1.x - n2.x) + abs(n1.y - n2.y);
	}
};

