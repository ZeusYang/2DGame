#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "BallObject.h"

// Game-related State data
SpriteRenderer  *Renderer;

// 初始化挡板的大小
const glm::vec2 PLAYER_SIZE(200, 40);
// 初始化挡板的速率
const GLfloat PLAYER_VELOCITY(500.0f);
//玩家挡板
GameObject      *Player;

// 初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const GLfloat BALL_RADIUS = 12.5f;
//运动的小球
BallObject     *Ball;

Game::Game(GLuint width, GLuint height)
	:State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game(){

}

void Game::Init(){
	// 加载着色器
	ResourceManager::LoadShader("../res/Shaders/Sprite.vert", 
		"../res/Shaders/Sprite.frag", nullptr, "sprite");
	// 配置着色器
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// 设置专用于渲染的控制
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/huaji.jpg", GL_TRUE, "face");
	ResourceManager::LoadTexture("../res/Textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("../res/Textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("../res/Textures/paddle.png", GL_TRUE, "paddle");
	// 加载关卡
	GameLevel one; one.Load("../res/levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("../res/levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three; three.Load("../res/levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four; four.Load("../res/levels/four.lvl", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 1;

	//玩家
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//小球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));
}

void Game::Update(GLfloat dt){
	 Ball->Move(dt, this->Width);
}

void Game::ProcessInput(GLfloat dt){
	if (this->State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// 移动挡板
		if (this->Keys[GLFW_KEY_A]) {//左移
			if (Player->Position.x >= 0)
				Player->Position.x -= velocity;
			if (Ball->Stuck)//小球随着挡板移动
				Ball->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D]) {//右移
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
			if (Ball->Stuck)//小球随着挡板移动
				Ball->Position.x += velocity;
		}
		if (this->Keys[GLFW_KEY_SPACE])//释放小球
			Ball->Stuck = false;
	}
}

void Game::Render(){
	if (this->State == GAME_ACTIVE){
		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// 绘制关卡
		this->Levels[this->Level].Draw(*Renderer);
		//玩家挡板和小球
		Player->Draw(*Renderer);
		Ball->Draw(*Renderer);
	}
}