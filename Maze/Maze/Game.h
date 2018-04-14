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
#include "GameObject.h"
#include "MazeAlgorithm.h"

//游戏状态
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_LOST,
	GAME_PAUSE
};

class Game
{
public:
	GLboolean		Keys[1024];//键盘响应
	GLboolean		KeysProcessed[1024];
	GLuint			Width, Height;//游戏宽高
	GameState		State;//游戏状态
	Game(GLuint width,GLuint height);
	~Game() = default;
	//游戏初始化
	void Init();
	//处理输入
	void ProcessInput(GLfloat dt);
	//游戏更新
	void Update(GLfloat dt);
	//渲染
	void Render();

private:
	//游戏控件
	std::shared_ptr<SpriteRenderer> sprite;
	std::shared_ptr<TextRenderer> text;
	std::shared_ptr<ISoundEngine> sound;
	std::shared_ptr<GameObject> wall;
	std::shared_ptr<GameObject> terminal;
	std::shared_ptr<GameObject> player;
	std::shared_ptr<MazeAlgorithm> maze;
	std::shared_ptr<ParticleGenerator> particle;
	//迷宫生成算法
	int type;
	//单元格子大小
	int unitX, unitY;
	GLuint gridX, gridY;
	//重置至初始状态
	void Reset();

};

