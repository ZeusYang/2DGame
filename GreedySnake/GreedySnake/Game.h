#pragma once
#include <memory>
#include <irrKlang.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace irrklang;
#include "Shader.h"
#include "Texture2D.h"
#include "TextRenderer.h"
#include "PostProcessor.h"
#include "ParticleGenerator.h"
#include "SnakeObject.h"
#include "GameObject.h"
#include "Algorithm.h"

//游戏状态
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_LOST
};

class Game
{
public:

	GLboolean		Keys[1024];//键盘响应
	GLboolean		KeysProcessed[1024];
	GLuint			Width, Height;//游戏宽高
	GameState		State;//游戏状态

	Game(GLuint width,GLuint height);
	~Game();
	//游戏初始化
	void Init();
	//处理输入
	void ProcessInput(GLfloat dt);
	//游戏更新
	void Update(GLfloat dt);
	//渲染
	void Render();
	//碰撞检测
	void DoCollisions();

private:

	//游戏控件
	std::shared_ptr<SpriteRenderer> sprite;
	std::shared_ptr<TextRenderer> text;
	std::shared_ptr<PostProcessor> effects;
	std::shared_ptr<ISoundEngine> sound;
	std::shared_ptr<SnakeObject> snake;
	std::shared_ptr<GameObject> food;
	std::shared_ptr<GameObject> firework;
	std::shared_ptr<ParticleGenerator> temptation, boom;
	//算法
	std::shared_ptr<Algorithm> algorithm;
	//单元格子大小
	int unitX, unitY;
	GLuint gridX, gridY;
	glm::vec2 InitVelocity;//初始蛇的速度
	double timer;//计时器
	double firetimer;
	//设置食物位置
	void SetFoodPos();
	
	//游戏模式
	int mode;
	//当前分数
	int score;

	//重置至初始状态
	void Reset();
};

