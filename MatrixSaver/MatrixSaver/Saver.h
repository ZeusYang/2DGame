#pragma once
#include "Shader.h"
#include "Texture2D.h"

class Saver{
public:
	GLboolean		Keys[1024];//键盘响应
	GLboolean		KeysProcessed[1024];
	GLuint			Width, Height;//窗口宽高

	Saver(GLuint width, GLuint height);

	~Saver() = default;

	//初始化
	void Init();
	//输入
	void ProcessInput(GLfloat dt);
	//更新
	void Update(GLfloat dt);
	//渲染
	void Render();

private:

};

