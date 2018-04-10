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
	~Game();
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
	std::shared_ptr<PostProcessor> effects;
	std::shared_ptr<ISoundEngine> sound;
	std::shared_ptr<SnakeObject> snake;
	std::shared_ptr<GameObject> food;
	std::shared_ptr<GameObject> firework;
	std::shared_ptr<ParticleGenerator> temptation, boom[3];
	std::shared_ptr<Algorithm> algorithm;//算法
	//单元格子大小
	int unitX, unitY;
	GLuint gridX, gridY;
	double firetimer[3];//烟花计时
	int fireindex;//索引到哪个烟花
	int score;//当前分数
	//重置至初始状态
	void Reset();
	//将二维数组转换成UI坐标，注意顺序是相反的
	inline glm::vec2 Index(const glm::ivec2 &index) {
		return glm::vec2(index.y*unitX, index.x*unitY);
	}
};

