#include "MazeAlgorithm.h"
#include <iterator>
#include <iostream>

std::vector<std::vector<Cell>> MazeAlgorithm::cells = std::vector<std::vector<Cell>>();

MazeAlgorithm::MazeAlgorithm(int r, int c, int ux, int uy)
	:row(r), col(c), unitX(ux), unitY(uy), frame(1), adventurer(0)
{
	//四个邻接墙的坐标
	dir[LEFT] = { glm::vec2(-unitX*0.5,+unitY*0.4),90.0f };
	dir[UP] = { glm::vec2(0 - 0.1*unitX,0),0.0f };
	dir[RIGHT] = { glm::vec2(+0.5*unitX,+unitY*0.4),90.0f };
	dir[DOWN] = { glm::vec2(0 - 0.1*unitX,unitY),0.0f };
	cells.resize(row, std::vector<Cell>(col));
	std::cout << cells.size() << std::endl;
	GetWallFromCells();
}

void MazeAlgorithm::Generation_Init() {
	paths.clear();
	solution.clear();
	//迷宫初始化，开始全部都是墙
	for (auto it = cells.begin(); it != cells.end(); ++it) {
		for (auto in = it->begin(); in != it->end(); ++in) {
			in->neighbors[LEFT] = in->neighbors[UP] = in->neighbors[RIGHT] = in->neighbors[DOWN] = 0;
			in->visited = false;
		}
	}
	//起始位置和终点位置无墙
	cells[0][0].neighbors[LEFT] = 1;
	cells[row - 1][col - 1].neighbors[RIGHT] = 1;
	start = Index(glm::ivec2(0, 0));
	end = Index(glm::ivec2(row - 1, col - 1));
	//冒险家
	adventurer = Index(glm::ivec2(0, 0));

	//dfs迷宫生成的辅助stack清空，然后加入第一个元素-->左上角
	while (!record.empty())record.pop();
	cur = glm::ivec2(0, 0);
	record.push(cur);

	//prim迷宫生成的辅助list清空，然后加入第一个元素-->左上角
	prim.clear();
	prim.push_back(cur);

	//四叉树分割生成的辅助queue清空，然后加入第一个元素
	while (!recursive.empty())recursive.pop();
	recursive.push({ glm::ivec2(0,row - 1),glm::ivec2(0,col - 1) });
}

void MazeAlgorithm::Pathfinding_Init() {
	//迷宫的visited初始化，寻路开始全部都是未访问的
	for (auto it = cells.begin(); it != cells.end(); ++it) {
		for (auto in = it->begin(); in != it->end(); ++in) {
			in->visited = 0;
			in->prev = glm::ivec2(-1, -1);
			in->inOpen = in->inClosed = false;
		}
	}

	//清空之前计算得到的路径
	solution.clear();

	//dfs迷宫寻路清空
	while (!path_dfs.empty())path_dfs.pop();
	path_dfs.push(glm::ivec2(0, 0));
	//bfs迷宫寻路清空
	while (!path_bfs.empty())path_bfs.pop();
	path_bfs.push(glm::vec2(0, 0));
	//A*迷宫寻路清空
	while (!path_astar.empty())path_astar.pop();
	path_astar.push(Node(0, 0));
	cells[0][0].inOpen = true;
	cells[0][0].gn = 0;
	cells[0][0].hn = DirectLen(glm::ivec2(0, 0), glm::ivec2(row - 1, col - 1));

	cells[0][0].visited = 1;
}

void MazeAlgorithm::GetWallFromCells() {
	walls.clear();
	for (auto x = 0; x < row; ++x) {
		for (auto y = 0; y < col; ++y) {
			//UI坐标下的左上角坐标
			glm::vec2 leftTop = Index(glm::ivec2(x, y));
			//std::cout << x << "," << y << std::endl;
			//因为有重复的，所以每个单元只算左、上
			if (cells[x][y].neighbors[LEFT] == 0)//0代表有墙
				walls.push_back(Wall(leftTop + dir[LEFT].first, dir[LEFT].second));
			if (cells[x][y].neighbors[UP] == 0)
				walls.push_back(Wall(leftTop + dir[UP].first, dir[UP].second));
			if (y == col - 1) {//最左边的
				if (cells[x][y].neighbors[RIGHT] == 0)
					walls.push_back(Wall(leftTop + dir[RIGHT].first, dir[RIGHT].second));
			}
			if (x == row - 1) {//最下边的
				if (cells[x][y].neighbors[DOWN] == 0)
					walls.push_back(Wall(leftTop + dir[DOWN].first, dir[DOWN].second));
			}
		}
	}
}

void MazeAlgorithm::GetPathFromCells() {
	paths.clear();
	for (auto x = 0; x < row; ++x) {
		for (auto y = 0; y < col; ++y) {
			if (cells[x][y].visited == 1) {//遍历过的路
				glm::vec2 leftTop = Index(glm::ivec2(x, y));
				leftTop += unitX*0.2;
				paths.push_back(leftTop);
			}
		}
	}
}

bool MazeAlgorithm::Generator_Dfs() {
	frame = 5;
	while (!record.empty() && frame--) {//当队列或者frame不减到0时
		cells[cur.x][cur.y].visited = 1;//标记当前的位置为访问过的了
		bool hasNeigh = false;//是否有邻居未访问
		std::vector<std::pair<glm::ivec2,int>> tmp; //记录未访问的邻居, tmp.second代表它是哪个邻居
		tmp.reserve(4);
		glm::ivec2 loc;
		//寻找是否存在未访问的邻居
		for (auto x = 0; x < 4; ++x) {
			loc = glm::ivec2(cur.x + to[x][0], cur.y + to[x][1]);
			if (CouldMove(loc) && !cells[loc.x][loc.y].visited) {//有未访问的邻居
				tmp.push_back({ loc,x });
				hasNeigh = true;
			}
		}
		if (hasNeigh) {//从未访问的邻居中随机选择一个
			int got = rand() % tmp.size();
			record.push(cur);//当前迷宫单元入栈
			//拆掉cur和tmp[got]之间的墙
			cells[cur.x][cur.y].neighbors[tmp[got].second] = 1;
			cells[tmp[got].first.x][tmp[got].first.y].neighbors[(tmp[got].second + 2) % 4] = 1;
			//令当前标记变为该邻居
			cur = tmp[got].first;
		}
		else {//没找到一个邻居，是时候回溯了
			cur = record.top();
			record.pop();
		}
	}
	//frame = 1;
	//栈尾空代表已经结束了
	if (record.empty())return true;
	else return false;
}

bool MazeAlgorithm::Generator_Prim() {
	frame = 5;
	//prim为list
	while (!prim.empty() && frame--) {
		//随机从list中选一个
		int choice = rand() % prim.size();
		auto it = prim.begin();
		std::advance(it, (choice == 0) ? 0 : choice);
		cur = *it;
		//标记为已访问过，然后从List删除
		cells[cur.x][cur.y].visited = 1;
		prim.erase(it);
		//查看邻居的情况
		std::vector<std::pair<glm::ivec2, int>> tmp; //记录未访问的邻居
		//四个邻居
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 loc = glm::ivec2(cur.x + to[x][0], cur.y + to[x][1]);
			if (CouldMove(loc)) {//边界检查
				//被访问过，加入tmp中，接下来要随机抽取这些
				if (cells[loc.x][loc.y].visited == 1)tmp.push_back(std::pair<glm::ivec2, int>(loc, x));
				else if (cells[loc.x][loc.y].visited == 0) {
					//未被访问过，加入list中，并标记为2，代表他们在list中
					prim.push_back(loc);
					cells[loc.x][loc.y].visited = 2;
				}
			}
		}
		//有被访问过的邻居
		if (!tmp.empty()) {
			//从中随机选一个，打通他们之间的墙
			int got = rand() % tmp.size();
			cells[cur.x][cur.y].neighbors[tmp[got].second] = 1;
			cells[tmp[got].first.x][tmp[got].first.y].neighbors[(tmp[got].second + 2) % 4] = 1;
		}
	}
	if (prim.empty())return true;
	else return false;
}

bool MazeAlgorithm::Generator_Recursive() {
	frame = 10;
	//recursive是queue，其中的元素为pair<glm::ivec2,glm::ivec2>
	//first为行范围，second为列范围
	while (!recursive.empty() && frame--) {
		std::pair<glm::ivec2, glm::ivec2> head = recursive.front();
		recursive.pop();
		//head.first == head.second情况下变成了一条线，需要特殊处理
		if (head.first.x < head.first.y && head.second.x < head.second.y) {
			glm::ivec2 center;
			//在[head.first,head.first)之间选择一个坐标，根据这个坐标进行分割
			center.x = head.first.x + rand() % (head.first.y - head.first.x);
			center.y = head.second.x + rand() % (head.second.y - head.second.x);
			int subRow[2], subCol[2];//存储四个方向上的随机数
			//在center四个方向上随机选取
			subRow[0] = head.first.x + rand() % (center.x - head.first.x + 1);
			subRow[1] = center.x + 1 + rand() % (head.first.y - center.x);
			subCol[0] = head.second.x + rand() % (center.y - head.second.x + 1);
			subCol[1] = center.y + 1 + rand() % (head.second.y - center.y);
			//获取四个方向上的随机迷宫单元
			glm::ivec2 meta[4];
			meta[LEFT] = glm::ivec2(center.x, subCol[0]);
			meta[UP] = glm::ivec2(subRow[0], center.y);
			meta[RIGHT] = glm::ivec2(center.x, subCol[1]);
			meta[DOWN] = glm::ivec2(subRow[1], center.y);
			int notOpen = rand() % 4;//随机选一个迷宫单元不打通，剩下的三个都打通
			for (auto x = 0; x < 4; ++x) {
				if (x != notOpen) {//在这三个迷宫单元挖洞
					//左、右打通它的下面，上、下打通它的右面
					glm::ivec2 near = (x % 2 == 0) ? glm::ivec2(meta[x].x + 1, meta[x].y) 
						: glm::ivec2(meta[x].x, meta[x].y + 1);
					//哪面墙
					int which = (x % 2 == 0) ? DOWN : RIGHT;
					//打通meta[x]和near之间的墙
					cells[meta[x].x][meta[x].y].neighbors[which] = 1;
					cells[near.x][near.y].neighbors[(which + 2) % 4] = 1;
				}
			}
			//然后再对当前的四个子空间进行同样处理，入队
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(head.first.x,center.x),
				glm::ivec2(head.second.x,center.y) }));
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(head.first.x,center.x),
				glm::ivec2(center.y + 1,head.second.y) }));
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(center.x + 1,head.first.y),
				glm::ivec2(head.second.x,center.y) }));
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(center.x + 1,head.first.y),
				glm::ivec2(center.y + 1,head.second.y) }));
		}
		else if (head.first.x < head.first.y) {//子空间变成了一条竖线，其他同上
			int rm = head.first.x + rand() % (head.first.y - head.first.x);
			cells[rm][head.second.x].neighbors[DOWN] = 1;
			cells[rm + 1][head.second.x].neighbors[UP] = 1;
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(head.first.x,rm),
				glm::ivec2(head.second.x,head.second.x) }));
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(rm + 1,head.first.y),
				glm::ivec2(head.second.x,head.second.x) }));
		}
		else if (head.second.x < head.second.y) {//子空间变成了一条横线，其他同上
			int cm = head.second.x + rand() % (head.second.y - head.second.x);
			cells[head.first.x][cm].neighbors[RIGHT] = 1;
			cells[head.first.x][cm+1].neighbors[LEFT] = 1;
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(head.first.x,head.first.x),
				glm::ivec2(head.second.x,cm) }));
			recursive.push(std::pair<glm::ivec2, glm::ivec2>({ glm::ivec2(head.first.x,head.first.x),
				glm::ivec2(cm + 1,head.second.y) }));
		}
	}
	if (recursive.empty()) return true;
	else return false;
}

bool MazeAlgorithm::Pathfinding_Dfs() {
	//深度优先遍历寻路
	frame = 3;
	while (!path_dfs.empty() && frame--) {
		glm::ivec2 head = path_dfs.top();
		path_dfs.pop();
		//寻找邻居
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 loc(head.x + to[x][0], head.y + to[x][1]);
			if (CouldMove(loc) && !cells[loc.x][loc.y].visited //未访问
				&& cells[head.x][head.y].neighbors[x] == 1) {//且无墙隔着
				cells[loc.x][loc.y].prev = head;//记录前驱，然后要倒推路径
				path_dfs.push(loc);
				cells[loc.x][loc.y].visited = 1;
				//找到终点了
				if (loc == glm::ivec2(row - 1, col - 1)) {
					//清空dfs栈，停止搜索
					while (!path_dfs.empty())path_dfs.pop();
				}
			}
		}
	}
	if (path_dfs.empty()) {//从终点倒退路径到起点
		GetSolution();
		return true;
	}
	else return false;
}

bool MazeAlgorithm::Pathfinding_Bfs() {
	//广度优先遍历寻路->最短路径
	frame = 3;
	while (!path_bfs.empty() && frame--) {
		glm::ivec2 head = path_bfs.front();
		path_bfs.pop();
		//寻找邻居
		for (auto x = 0; x < 4; ++x) {
			glm::ivec2 loc(head.x + to[x][0], head.y + to[x][1]);
			if (CouldMove(loc) && !cells[loc.x][loc.y].visited //未访问
				&& cells[head.x][head.y].neighbors[x] == 1) {//且无墙隔着
				cells[loc.x][loc.y].prev = head;//记录前驱，然后要倒推路径
				path_bfs.push(loc);
				cells[loc.x][loc.y].visited = 1;
				//找到终点了
				if (loc == glm::ivec2(row - 1, col - 1)) {
					//清空dfs栈，停止搜索
					while (!path_bfs.empty())path_bfs.pop();
				}
			}
		}
	}
	if (path_bfs.empty()) {//从终点倒退路径到起点
		GetSolution();
		return true;
	}
	else return false;
}

bool MazeAlgorithm::Pathfinding_Astar() {
	frame = 3;
	while (!path_astar.empty() && frame--) {
		Node head = path_astar.top();
		path_astar.pop();
		//标记为放入closed表
		cells[head.index.x][head.index.y].inOpen = false;
		cells[head.index.x][head.index.y].inClosed = true;
		cells[head.index.x][head.index.y].visited = 1;
		////找到终点了
		if (head.index == glm::ivec2(row - 1, col - 1)) {
			//清空queue，停止搜索
			while (!path_astar.empty())path_astar.pop();
			break;
		}
		for (auto x = 0; x < 4; ++x) {//查看邻居
			glm::ivec2 loc(head.index.x + to[x][0], head.index.y + to[x][1]);
			if (CouldMove(loc) && cells[head.index.x][head.index.y].neighbors[x] == 1) {//无墙隔着
				if (cells[loc.x][loc.y].inClosed)continue;//已在closed表中，不管它
				if (!cells[loc.x][loc.y].inOpen) {//不在open表中，加入open表
					path_astar.push(Node(loc.x, loc.y));
					cells[loc.x][loc.y].inOpen = true;
					cells[loc.x][loc.y].prev = head.index;
					cells[loc.x][loc.y].gn = cells[head.index.x][head.index.y].gn + 1;
					cells[loc.x][loc.y].hn = DirectLen(loc, glm::ivec2(row - 1, col - 1));
				}
				else {//已在open表中，我们进行比较，然后修改前驱
					int orig = cells[loc.x][loc.y].gn + cells[loc.x][loc.y].hn;
					int nows = cells[head.index.x][head.index.y].gn + cells[loc.x][loc.y].hn + 1;
					if (nows < orig) {
						cells[loc.x][loc.y].prev = head.index;
						cells[loc.x][loc.y].gn = cells[head.index.x][head.index.y].gn + 1;
						cells[loc.x][loc.y].hn = cells[loc.x][loc.y].hn;
					}
				}
			}
		}
	}
	if (path_astar.empty()) {//从终点倒退路径到起点
		GetSolution();
		return true;
	}
	else return false;
}

void MazeAlgorithm::GetSolution() {
	//从终点倒退路径到起点
	glm::ivec2 index(row - 1, col - 1);
	while (index != glm::ivec2(-1, -1)) {
		solution.push_back(Index(index) + unitX*0.2f);
		index = cells[index.x][index.y].prev;
	}
	adven_index = solution.size() - 1;
}

bool MazeAlgorithm::AdventurerMove() {
	if (adven_index < 0 || solution.empty())return true;
	adventurer = solution[adven_index];
	--adven_index;
	return false;
}