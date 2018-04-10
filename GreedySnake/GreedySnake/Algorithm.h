#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <queue>
#include <list>

//结点类，用于bfs搜索
struct Node {
	std::pair<int, int> index;
	int hn, gn;//启发式函数
	Node(int x, int y, int h, int g) :index(std::pair<int, int>(x, y)), hn(h), gn(g) {}
	bool operator>(const Node &n)const {
		return (this->hn + this->gn) > (n.hn + n.gn);
	}
	bool operator<(const Node &n)const {
		return (this->hn + this->gn) < (n.hn + n.gn);
	}
};

//状态，用记录二维数组中每个点的状态，是否被访问过并记录他们的前驱
struct State {
	std::pair<int, int> prev;//前驱，前驱为-1代表没有前驱
	int visited;//记录是否访问过
	State() :prev(std::pair<int, int>(-1, -1)), visited(0) {}
};

//二维数组的结点向上、下、左、右四个扩展方向
const int dir[][2] = {
	{-1,0},{+1,0},{0,-1},{0,+1}
};

//算法类
class Algorithm
{
public:
	Algorithm(GLuint x,GLuint y);
	~Algorithm();

	//判断能否在此处放置食物
	bool CouldPlaceFood(GLuint x, GLuint y);
	//重置
	void Reset();
	//设置数组中(y,x)为1，代表这个位置被蛇占了
	void Place(GLuint x, GLuint y);

	//人工智能部分
	//bfs+优选队列寻找一条最优路径
	bool Search(std::pair<int, int> head, std::pair<int, int> dest,int type);
	//随便找一条路
	bool JustFindOne(std::pair<int, int> head);
	//判断蛇走这一步安不安全
	bool IsSafe(std::pair<int,int> head,std::pair<int,int> tail, std::pair<int,int> tailPrev, bool isFood);
	//路径对，存储蛇移动下一步的路径
	std::pair<int,int> path;

	void Display();
	
private:
	GLuint gridX, gridY;
	std::vector<std::vector<int> >board;

	//计算两个下标之间的直线距离
	int DirectLen(const std::pair<int, int> &n1, const std::pair<int, int> &n2) {
		return std::sqrt((n1.first - n2.first)*(n1.first - n2.first)
			+ (n1.second - n2.second)*(n1.second - n2.second));
	}
};

