#include "MazeAlgorithm.h"

MazeAlgorithm::MazeAlgorithm(int r, int c, int ux, int uy)
	:row(r), col(c), unitX(ux), unitY(uy), frame(1)
{
	cells.resize(row, std::vector<Cell>(col));
	//四个邻接墙的坐标
	dir[LEFT] = { glm::vec2(-unitX*0.5,+unitY*0.4),90.0f };
	dir[UP] = { glm::vec2(0 - 0.1*unitX,0),0.0f };
	dir[RIGHT] = { glm::vec2(+0.5*unitX,+unitY*0.4),90.0f };
	dir[DOWN] = { glm::vec2(0 - 0.1*unitX,unitY),0.0f };
	GetWallFromCells();
}

void MazeAlgorithm::Init() {
	//迷宫初始化，开始全部都是墙
	for (auto it = cells.begin(); it != cells.end(); ++it) {
		for (auto in = it->begin(); in != it->end(); ++in) {
			in->neighbors[LEFT] = in->neighbors[UP] = in->neighbors[RIGHT] = in->neighbors[DOWN] = 0;
			in->visited = false;
		}
	}
	//dfs迷宫生成的辅助stack清空，然后加入第一个元素-->左上角
	while (!record.empty())record.pop();
	cur = glm::ivec2(0, 0);
	record.push(cur);
}

void MazeAlgorithm::GetWallFromCells() {
	walls.clear();
	for (auto x = 0; x < row; ++x) {
		for (auto y = 0; y < col; ++y) {
			//UI坐标下的左上角坐标
			glm::vec2 leftTop = Index(glm::ivec2(x, y));
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

bool MazeAlgorithm::Generator_Dfs() {
	frame = 10;
	while (!record.empty() && frame--) {//当队列或者frame不减到0时
		cells[cur.x][cur.y].visited = true;//标记当前的位置为访问过的了
		bool hasNeigh = false;//是否有邻居未访问
		std::vector<std::pair<glm::ivec2,int>> tmp; //记录未访问的邻居
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
	frame = 1;
	//栈尾空代表已经结束了
	if (record.empty())return true;
	else return false;
}