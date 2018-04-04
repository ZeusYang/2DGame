#pragma once
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "GameLevel.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	//当前游戏状态
	GameState		State;
	GLboolean		Keys[1024];//键盘响应
	GLuint			Width, Height;//游戏宽高
	//游戏关卡
	std::vector<GameLevel> Levels;
	GLuint                 Level;

	Game(GLuint width, GLuint height);
	~Game();
	//游戏初始化
	void Init();
	//游戏循环
	void ProcessInput(GLfloat dt);//处理输入
	void Update(GLfloat dt);//更新
	void Render();//渲染
};

