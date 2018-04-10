#include "Algorithm.h"
#include <algorithm>
#include <iostream>
#include <functional>

Algorithm::Algorithm(GLuint x, GLuint y)
	:gridX(x),gridY(y)
{
	board.resize(gridX, std::vector<int>(gridY, 0));
}

Algorithm::~Algorithm()
{
}

bool Algorithm::CouldPlaceFood(GLuint x, GLuint y){
	return board[x][y] == 0;
}

void Algorithm::Reset() {
	for (auto &t : board) {
		std::fill(t.begin(), t.end(), 0);
	}
}

void Algorithm::Display() {
	for (auto &t : board) {
		for (auto &x : t) {
			std::cout << x << "-";
		}
		std::cout << std::endl;
	}
}

void Algorithm::Place(GLuint x, GLuint y) {
	board[y][x] = 1;
}

bool Algorithm::Search(std::pair<int, int> head, std::pair<int, int> dest, int type) {
	//启发式函数f(n) = g(n) + h(n)
	//g(n)为head到当前节点的实际距离，h(n)为当前节点到目的的欧拉直线距离
	if (type == 2)board[dest.first][dest.second] = 0;
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>>record;//最大堆
	Node first(head.first,head.second,0, DirectLen(head,dest));
	record.push(first);
	//用于逆推路径，记录每个点的前驱
	std::vector<std::vector<State>> prev;
	//初始化，此时每个节点的前驱为-1，代表没有前驱
	prev.resize(this->gridX, std::vector<State>(this->gridY));
	//起点已经被加进队列中了
	prev[head.first][head.second].visited = 1;
	//记录是否能够找到一条从起点到目的地的路径
	bool flag = false;
	while (!record.empty()) {
		Node cur = record.top();
		record.pop();
		//结点向四个方向扩展
		for (auto x = 0; x < 4; ++x) {
			std::pair<int, int> loc = cur.index;
			loc.first += dir[x][0];
			loc.second += dir[x][1];
			//边界判断
			if (loc.first < 0 || loc.first >= gridX) continue;
			if (loc.second < 0 || loc.second >= gridY) continue;
			//如果这个位置有蛇身或者先前已经访问过了，直接跳过
			if (prev[loc.first][loc.second].visited == 1 
				|| board[loc.first][loc.second] == 1)continue;
			//记录这个位置已经访问过了并记录它的前驱
			prev[loc.first][loc.second].visited = 1;
			prev[loc.first][loc.second].prev = cur.index;
			record.push(Node(loc.first, loc.second, cur.hn + 1, DirectLen(loc, dest)));
			//如果已经到了目的地，则停止搜索
			if (loc == dest) {
				flag = true;
				if(type != 2)break;//type == 2是搜索蛇尾的，寻找最长路径
			}
		}
	}
	if (type == 2)board[dest.first][dest.second] = 1;
	if (!flag) return false;
	if (type == 3)return true;
	//找到了路，从目的开始逆推
	std::pair<int, int> in = dest;
	while (prev[in.first][in.second].prev != head){
		in = prev[in.first][in.second].prev;
	}
	path = in;
	return true;
}

//判断蛇走这一步安不安全
bool Algorithm::IsSafe(std::pair<int, int> head, std::pair<int, int> tail,
	std::pair<int, int> tailPrev, bool isFood) {
	std::pair<int, int> realTail;
	//虚拟地走一步
	realTail = (!isFood) ? tailPrev : tail;
	//realTail = tail;
	board[path.first][path.second] = 1;
	board[realTail.first][realTail.second] = 0;
	board[tail.first][tail.second] = 0;
	bool ret = Search(path, realTail, 3);
	//恢复原位
	board[path.first][path.second] = 0;
	board[realTail.first][realTail.second] = 1;
	board[tail.first][tail.second] = 1;
	//移动后头和尾不能连一起
	//int ds = abs(realTail.first - path.first) + abs(realTail.second - path.second);
	//if (ds == 1)std::cout << "very unsafe\n";
	return ret;
}

bool Algorithm::JustFindOne(std::pair<int, int> head) {
	for (auto x = 0; x < 4; ++x) {
		//边界处理
		if (head.first + dir[x][0] < 0 || head.first + dir[x][0] >= gridX) continue;
		if (head.second + dir[x][1] < 0 || head.second + dir[x][1] >= gridY) continue;
		//随便一个方向走，只要没被霸占的地方都行
		if (board[head.first + dir[x][0]][head.second + dir[x][1]] == 0) {
			path = (std::pair<int, int>(head.first + dir[x][0], head.second + dir[x][1]));
			return true;
		}
	}
	return false;//穷途末路
}

