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
	//board[y][x] <= 0代表这个位置没被蛇身占位
	return board[y][x] <= 0;
}

void Algorithm::Reset() {
	for (auto &t : board) {
		std::fill(t.begin(), t.end(), 0);
	}
}

void Algorithm::Place(GLuint x, GLuint y) {
	board[y][x] = 1;
}

void Algorithm::Display() {
	for (auto &t : board) {
		for (auto &u : t) {
			std::cout << u << "-";
		}
		std::cout << std::endl;
	}
	std::cout << "----------------------\n";
}

bool Algorithm::Brute_force(std::pair<int, int> head, std::pair<int, int> dest){
	//启发式函数f(n) = g(n) + h(n)
	//g(n)为head到当前节点的实际距离，h(n)为当前节点到目的的欧拉直线距离
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>>record;
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
			if (loc.first < 0 || loc.first >= gridX)continue;
			if (loc.second < 0 || loc.second >= gridY)continue;
			//如果这个位置有蛇身或者先前已经访问过了，直接跳过
			if (prev[loc.first][loc.second].visited == 1 || board[loc.first][loc.second] == 1)continue;//有bug
			//记录这个位置已经访问过了
			prev[loc.first][loc.second].visited = 1;
			//记录前驱
			prev[loc.first][loc.second].prev = cur.index;
			//加入队列中
			record.push(Node(loc.first, loc.second, cur.hn + 1, DirectLen(loc, dest)));
			//如果已经到了目的地，则停止搜索
			if (loc == dest) {
				flag = true;
				//std::cout << "Yes" << std::endl;
				//std::cout << loc.first << "," << loc.second << std::endl;
				break;
			}
		}
	}
	//没有找到路,向四个方向瞎走
	if (!flag) {
		for (auto x = 0; x < 4; ++x) {
			if (board[head.first + dir[x][0]][head.second + dir[x][1]] == 0) {
				path.push_front(std::pair<int, int>(head.first + dir[x][0],
					head.second + dir[x][1]));
				return true;
			}
		}
		//真的一步都走不了，直接不动了
		return false;
	}
	//找到了路，从目的开始逆推
	path.clear();
	std::pair<int, int> in = dest;
//	std::cout << "find path:\n";
	while (prev[in.first][in.second].prev != std::pair<int, int>(-1, -1)) 
	{
		path.push_front(in);
		//std::cout << in.first << "," << in.second << "->";
		in = prev[in.first][in.second].prev;
	}
	std::cout << std::endl;
	return true;
}