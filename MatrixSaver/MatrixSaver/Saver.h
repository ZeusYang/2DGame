#pragma once
#include "Shader.h"
#include "Texture2D.h"
#include "CharList.h"
#include "SpriteRenderer.h"
#include <list>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

class Saver{
public:
	GLboolean		Keys[1024];//键盘响应
	GLboolean		KeysProcessed[1024];
	GLuint			Width, Height;//窗口宽高
	float znear, zfar, aspect, fovy;

	Saver(GLuint width = 800, GLuint height = 600,
		GLfloat zn = 1.0f, GLfloat zf = 70.0f, GLfloat ap = 1.0, GLfloat fy = 45.0f);

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
	std::vector<CharList*> matrix;
	std::shared_ptr<SpriteRenderer> sprite;
	float timeCounter;
};

