#include "GameLevel.h"

void GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight){
	// 清空过期数据
	this->Bricks.clear();
	// 从文件中加载
	GLuint tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);//文件流
	std::vector<std::vector<GLuint>> tileData;
	if (fstream){//一行一行读取
		while (std::getline(fstream, line)) // 读取关卡文件的每一行
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode) // 读取被空格分隔的每个数字
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)//由砖块数据初始化关卡
			this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint lvlWidth, GLuint lvlHeight){
	// 计算每个维度的大小
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
	//均匀分配每个砖块的大小
	GLfloat unit_width = lvlWidth / static_cast<GLfloat>(width);
	GLfloat unit_height = lvlHeight / height;
	// 基于tileDataC初始化关卡     
	for (GLuint y = 0; y < height; ++y){
		for (GLuint x = 0; x < width; ++x){
			// 检查砖块类型
			if (tileData[y][x] == 1){//不可摧毁的砖块
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size,
					ResourceManager::GetTexture("block_solid"),
					glm::vec3(0.8f, 0.8f, 0.7f)
				);
				obj.IsSolid = GL_TRUE;
				this->Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1){//一个可被摧毁的砖块
				glm::vec3 color = glm::vec3(1.0f); // 默认为白色
				//不同的数字区分砖块的颜色
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				this->Bricks.push_back(
					GameObject(pos, size, ResourceManager::GetTexture("block"), color)
				);
			}
		}
	}
}

void GameLevel::Draw(SpriteRenderer &renderer){
	for (GameObject &tile : this->Bricks)//渲染所有未被破坏的砖块
		if (!tile.Destroyed)
			tile.Draw(renderer);
}

GLboolean GameLevel::IsCompleted(){
	for (GameObject &tile : this->Bricks)//遍历检测所有砖块是否全部被破坏
		if (!tile.IsSolid && !tile.Destroyed)
			return GL_FALSE;
	return GL_TRUE;
}