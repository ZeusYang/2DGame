#include "Game.h"
#include "CollisionDetect.h"
#include "ResourceManager.h"
#include "Menu.h"
#include <iostream>
#include <sstream>

Game::Game(GLuint width, GLuint height)
	:Width(width),Height(height),State(GAME_MENU), unitX(30), unitY(30),
	gridX(Width /unitX),gridY(Height/unitY),fireindex(0),score(0)
{
	firetimer[0] = firetimer[1] = firetimer[2] = 0;
	algorithm = std::make_shared<Algorithm>(this->gridY, this->gridX);
}

Game::~Game()
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

	//游戏控件
	sound = std::shared_ptr<ISoundEngine>(createIrrKlangDevice());
	sprite = std::make_shared<SpriteRenderer>(ResourceManager::GetShader("sprite"));

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.png", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/particle.bmp", GL_FALSE, "snakebody");
	ResourceManager::LoadTexture("../res/Textures/head.png", GL_FALSE, "snakehead");
	ResourceManager::LoadTexture("../res/Textures/Light.png", GL_FALSE, "particle");

	//蛇
	glm::vec2 headPos = glm::vec2(
		(gridX - 1) / 2 * unitX, (gridY - 1) / 2 * unitY
	);
	snake = std::make_shared<SnakeObject>(headPos, unitX / 2, glm::vec2(unitX,unitY),
		ResourceManager::GetTexture("snakebody"));

	//食物
	food = std::make_shared<GameObject>(glm::vec2(0.0, 0.0), glm::vec2(unitX, unitY),
		ResourceManager::GetTexture("snakebody"));
	food->Velocity = glm::vec2(300, 300);
	food->Position = Index(algorithm->food->Index);
	firework = std::make_shared<GameObject>(glm::vec2(0.0, 0.0), glm::vec2(unitX, unitY),
		ResourceManager::GetTexture("snakebody"));
	firework->Velocity = glm::vec2(200, 200);
	//后处理特效
	effects = std::make_shared<PostProcessor>(ResourceManager::GetShader("postprocessing"),
		this->Width, this->Height);
	//文本显示
	text = std::make_shared<TextRenderer>(this->Width, this->Height);
	text->Load("../res/Fonts/ocraext.TTF", 24);
	//背景音乐
	sound->play2D("../res/Audio/greedysnake.mp3", GL_TRUE);
	//粒子特效
	temptation = std::make_shared<ParticleGenerator>(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		800,//粒子数量
		unitX,//粒子大小
		1.5f,//粒子最终寿命
		1.0/1.5f//透明度衰减速度
		);
	for(int x = 0;x < 3;++x)
		boom[x] = std::make_shared<ParticleGenerator>(
			ResourceManager::GetShader("particle"),
			ResourceManager::GetTexture("particle"),
			410,//粒子数量
			unitX*1.1,//粒子大小
			2.0f,//粒子最终寿命
			1.0f/2.0f//透明度衰减速度
			);
	this->Reset();
}

//处理输入
void Game::ProcessInput(GLfloat dt) {
	if (this->State == GAME_MENU) {
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {//确定
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			Reset();//重置
		}
	}
	if (this->State == GAME_LOST || this->State == GAME_WIN) {//游戏输赢之后按确定重新开始
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]){
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			this->State = GAME_MENU;
			Reset();
		}
	}
	if (this->State == GAME_ACTIVE) {
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P]) {//暂停
			this->State = GAME_PAUSE;
			this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
			algorithm->Display();
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
	
	if (this->State == GAME_ACTIVE ) {//AI模式
		//AI策略
		glm::ivec2 move = algorithm->AIThinking();
		//没找到任何路径，游戏结束
		if (move == glm::ivec2(-1, -1))this->State = GAME_LOST;
		else {
			//走出一步
			bool isCollision = algorithm->make_move(move);
			if (isCollision) {//碰撞,boom
				fireindex = (fireindex + 1) % 3;
				firetimer[fireindex] = 2.0f;
				firework->Position = food->Position;
				boom[fireindex]->Reset();
				boom[fireindex]->Update(0.f, *firework, 400, firework->Size / 2.0f, 3, fireindex);
				sound->play2D("../res/Audio/get.wav", GL_FALSE);
				//获取一分
				++score;
			}
			if (algorithm->win) {//满屏了
				State = GAME_WIN;
				return;
			}
			food->Position = Index(algorithm->food->Index);//更新食物位置
		}
	}
	//粒子更新
	temptation->Update(dt, *food, 8, food->Size / 20.f, 2);
	//烟花爆炸
	for (int x = 0; x < 3; ++x) {
		if (firetimer[x] > 0) {
			firetimer[x] -= dt;
			boom[x]->Update(dt, *firework, 0, firework->Size / 2.0f, 3, fireindex);
		}
	}
	if (firetimer[fireindex] <= 0 && State != GAME_ACTIVE && State != GAME_PAUSE) {//非激活状态随机位置绽放烟花
		firetimer[fireindex] = 2.0f;
		//随机位置
		int x1 = rand() % gridX;
		int y1 = rand() % gridY;
		firework->Position = glm::vec2(x1*unitX, y1*unitY);
		boom[fireindex]->Reset();
		boom[fireindex]->Update(0.f, *firework, 400, firework->Size / 2.0f, 3, fireindex);
		fireindex = (fireindex + rand()%3) % 3;
	}
}

//渲染
void Game::Render() {
	std::stringstream ss; ss << this->score;
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU
		|| this->State == GAME_WIN || this->State == GAME_LOST || this->State == GAME_PAUSE) {
		// 绘制背景
		sprite->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//蛇身
		snake->DrawSnake(algorithm->snake, *sprite, ResourceManager::GetTexture("snakehead"));
		//食物
		food->Draw(*sprite);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//粒子
		temptation->Draw();
		//烟花爆炸
		if(firetimer[0]>0)boom[0]->Draw();
		if (firetimer[1]>0)boom[1]->Draw();
		if (firetimer[2]>0)boom[2]->Draw();
		//分数
		text->RenderText("Score:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU) {
		Menu::Selection(*text, this->Width, this->Height);
	}
	if (this->State == GAME_WIN) {
		Menu::Win(*text, this->Width, this->Height);
	}
	if(this->State == GAME_LOST){
		//分数
		Menu::Lost(*text, this->Width, this->Height, ss.str());
	}
}

//重置至初始状态
void Game::Reset(){
	fireindex = 0;
	firetimer[0] = firetimer[1] = firetimer[2] = 0.0f;
	score = 0;
	algorithm->ResetSnakeAndFood();
	food->Position = Index(algorithm->food->Index);
	snake->Position = Index(algorithm->snake.front().Index);
}
