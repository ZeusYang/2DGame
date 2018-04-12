#include "Game.h"
#include "ResourceManager.h"
#include "Menu.h"
#include <iostream>
#include <sstream>

Game::Game(GLuint width, GLuint height)
	:Width(width),Height(height),State(GAME_MENU), unitX(15), unitY(15),
	gridX(Width /unitX),gridY(Height/unitY)
{
	maze = std::make_shared<MazeAlgorithm>(gridY - 1, gridX - 1, unitX, unitY);
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
	wall = std::make_shared<GameObject>(glm::vec2(0, 0), glm::vec2(unitX, unitY*0.2f),
		ResourceManager::GetTexture("wall"), glm::vec3(1.0, 0.0, 0.0));

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.png", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/brick.png", GL_FALSE, "wall");
	ResourceManager::LoadTexture("../res/Textures/Light.png", GL_FALSE, "particle");

	//后处理特效
	effects = std::make_shared<PostProcessor>(ResourceManager::GetShader("postprocessing"),
		this->Width, this->Height);
	//文本显示
	text = std::make_shared<TextRenderer>(this->Width, this->Height);
	text->Load("../res/Fonts/ocraext.TTF", 24);
	//背景音乐
	sound->play2D("../res/Audio/background.mp3", GL_TRUE);
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
			//Reset();
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
	
	if (this->State == GAME_ACTIVE ) {//激活模式
		maze->GetWallFromCells();//获取砖墙的位置
		if (maze->Generator_Dfs()) {//生成迷宫，一点点生成
			this->State = GAME_WIN;
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

		//分数
		//text->RenderText("Score:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU) {
		Menu::Selection(*text, this->Width, this->Height);
	}
	if (this->State == GAME_WIN) {
		Menu::Win(*text, this->Width, this->Height);
	}
	if(this->State == GAME_LOST){
		//分数
		Menu::Lost(*text, this->Width, this->Height, std::string("Over"));
	}
}

//重置至初始状态
void Game::Reset(){
	maze->Init();
	//firetimer[0] = firetimer[1] = firetimer[2] = 0.0f;
	//algorithm->ResetSnakeAndFood();
	//food->Position = Index(algorithm->food->Index);
	//snake->Position = Index(algorithm->snake.front().Index);
}
