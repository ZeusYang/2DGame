#include "Game.h"
#include "ResourceManager.h"
#include "Menu.h"
#include <iostream>
#include <sstream>

Game::Game(GLuint width, GLuint height)
	:Width(width),Height(height),State(GAME_MENU), unitX(30), unitY(30),//30
	gridX(Width /unitX),gridY(Height/unitY),type(1)
{
}

//游戏初始化
void Game::Init() {
	// 加载着色器
	ResourceManager::LoadShader("../res/Shaders/Sprite.vert",
		"../res/Shaders/Sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("../res/Shaders/Particle.vert",
		"../res/Shaders/Particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("../res/Shaders/Effect.vert",
		"../res/Shaders/Effect.frag", nullptr, "postprocessing");

	// 配置着色器
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/brick.png", GL_FALSE, "wall");
	ResourceManager::LoadTexture("../res/Textures/Star.bmp", GL_FALSE, "terminal");
	ResourceManager::LoadTexture("../res/Textures/Light.png", GL_FALSE, "particle");
	ResourceManager::LoadTexture("../res/Textures/head.png", GL_FALSE, "head");
	ResourceManager::LoadTexture("../res/Textures/foot.png", GL_FALSE, "foot");
	
	//游戏控件
	sound = std::shared_ptr<ISoundEngine>(createIrrKlangDevice());
	sprite = std::make_shared<SpriteRenderer>(ResourceManager::GetShader("sprite"));
	wall = std::make_shared<GameObject>(glm::vec2(0, 0), glm::vec2(unitX, unitY*0.2f),
		ResourceManager::GetTexture("wall"), glm::vec3(0.7, 0.7, 0.7));
	terminal = std::make_shared<GameObject>(glm::vec2(0, 0), glm::vec2(unitX*1.2f, unitY*1.2f),
		ResourceManager::GetTexture("terminal"), glm::vec3(1.0, 1.0, 1.0));
	player = std::make_shared<GameObject>(glm::vec2(0, 0), glm::vec2(unitX*0.8f, unitY*0.8f),
		ResourceManager::GetTexture("particle"), glm::vec3(1.0, 1.0, 1.0));
	player->Velocity = glm::vec2(unitX * 8, unitY * 8);

	//文本显示
	text = std::make_shared<TextRenderer>(this->Width, this->Height);
	text->Load("../res/Fonts/ocraext.TTF", 24);
	//背景音乐
	sound->play2D("../res/Audio/background.mp3", GL_TRUE);
	//粒子效果
	particle = std::make_shared<ParticleGenerator>(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		1000,
		40.0f
	);

	maze = std::make_shared<MazeAlgorithm>(gridY - 1, gridX - 1, unitX, unitY);
	maze->Generation_Init();
}

//处理输入
void Game::ProcessInput(GLfloat dt) {
	if (this->State == GAME_MENU) {
		if (this->Keys[GLFW_KEY_1])type = 1;
		if (this->Keys[GLFW_KEY_2])type = 2;
		if (this->Keys[GLFW_KEY_3])type = 3;
		if (this->Keys[GLFW_KEY_A] && !this->KeysProcessed[GLFW_KEY_A]) {
			type = 4;
			maze->Pathfinding_Init();
			this->KeysProcessed[GLFW_KEY_A] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_B] && !this->KeysProcessed[GLFW_KEY_B]) {
			type = 5;
			maze->Pathfinding_Init();
			this->KeysProcessed[GLFW_KEY_B] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_C] && !this->KeysProcessed[GLFW_KEY_C]) {
			type = 6;
			maze->Pathfinding_Init();
			this->KeysProcessed[GLFW_KEY_C] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {//确定
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			if(type <= 3)Reset();//迷宫生成重置
			else maze->Pathfinding_Init();//寻路重置
		}
	}
	if (this->State == GAME_LOST || this->State == GAME_WIN) {//游戏输赢之后按确定重新开始
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]){
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			this->State = GAME_MENU;
		}
	}
	if (this->State == GAME_ACTIVE) {
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P]) {//暂停
			this->State = GAME_PAUSE;
			this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
		}
	}
	if (this->State == GAME_PAUSE) {
		if (this->Keys[GLFW_KEY_B]) {
			this->State = GAME_ACTIVE;
		}
	}
}

//游戏更新
void Game::Update(GLfloat dt) {
	if (type <= 3)maze->GetWallFromCells();//获取砖墙的位置
	else {
		maze->GetPathFromCells();//获取遍历路径的位置
	}
	player->Position = maze->adventurer;
	particle->Update(dt, *player, 10, player->Size / 4.0f, 2);
	if (this->State == GAME_ACTIVE ) {//激活模式
		switch (type) {
		case 1:
			if (maze->Generator_Dfs()) {//dfs生成迷宫，一点点生成
				this->State = GAME_WIN;
			}
			break;
		case 2:
			if (maze->Generator_Prim()) {//prim随机生成迷宫算法s
				this->State = GAME_WIN;
			}
			break;
		case 3:
			if (maze->Generator_Recursive()) {//四叉树随机分割生成迷宫算法
				this->State = GAME_WIN;
			}
			break;
		case 4://dfs
			if (maze->Pathfinding_Dfs())type = 7;//type == 7进入路径演示状态
			break;
		case 5://bfs
			if (maze->Pathfinding_Bfs()) type = 8;//type == 8进入路径演示状态
			break;
		case 6://Astart
			if (maze->Pathfinding_Astar()) type = 9;//type == 9进入路径演示状态
			break;
		case 7:
			if (maze->AdventurerMove()) {
				type = 4;
				this->State = GAME_WIN;
			}
			break;
		case 8:
			if (maze->AdventurerMove()) {
				type = 5;
				this->State = GAME_WIN;
			}
			break;
		case 9:
			if (maze->AdventurerMove()) {
				type = 6;
				this->State = GAME_WIN;
			}
			break;
		default:
			break;
		}
	}
}

//渲染
void Game::Render() {
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU
		|| this->State == GAME_WIN || this->State == GAME_LOST || this->State == GAME_PAUSE) {
		// 绘制背景
		sprite->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);

		//绘制迷宫
		Texture2D &tmp = ResourceManager::GetTexture("wall");
		for (auto it = maze->walls.begin(); it != maze->walls.end(); ++it) {
			sprite->DrawSprite(tmp, it->Pos, wall->Size, glm::radians(it->angle), wall->Color);
		}

		//绘制遍历路径
		Texture2D &tmp1 = ResourceManager::GetTexture("wall");
		for (auto it = maze->paths.begin(); it != maze->paths.end(); ++it) {
			sprite->DrawSprite(tmp1, *it, glm::vec2(unitX*0.8, unitY*0.8), 0, glm::vec3(0.0, 1.0, 0.0));
		}

		for (auto it = maze->solution.begin(); it != maze->solution.end(); ++it) {
			sprite->DrawSprite(ResourceManager::GetTexture("foot"),
				*it, glm::vec2(unitX*0.8, unitY*0.8), 0, glm::vec3(5.0, 0.0, 0.0));
		}

		//绘制冒险家
		sprite->DrawSprite(ResourceManager::GetTexture("head") ,
			maze->adventurer, glm::vec2(unitX*0.8, unitY*0.8), 0, glm::vec3(0.0, 0.0, 1.0));
		//glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//起点和终点
		sprite->DrawSprite(terminal->Sprite,
			maze->start, terminal->Size, 0.0f, glm::vec3(2.0, 0.0, 0.0));
		sprite->DrawSprite(terminal->Sprite,
			maze->end, terminal->Size, 0.0f, glm::vec3(0.0, 0.0, 2.0));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		particle->Draw();
	}
	if (this->State == GAME_MENU) {
		Menu::Selection(*text, this->Width, this->Height);
	}
	if (this->State == GAME_WIN) {
		Menu::Win(*text, this->Width, this->Height);
	}
}

//重置至初始状态
void Game::Reset(){
	maze->Generation_Init();
}
