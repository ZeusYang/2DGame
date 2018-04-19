#include "Saver.h"
#include "ResourceManager.h"
#include <iostream>
#include <algorithm>
#include <functional>

Saver::Saver(GLuint width, GLuint height, GLfloat zn, GLfloat zf, GLfloat ap, GLfloat fy)
	:Width(width), Height(height), znear(zn), zfar(zf), aspect(ap), fovy(fy),
	timeCounter(0.0)
{
}

//初始化
void Saver::Init() {
	// 加载着色器
	std::string vShader = 
		"#version 330 core\n"
		"layout(location = 0) in vec3 vertex;\n"
		"layout(location = 1) in vec2 texcoord;\n"
		"layout(location = 2) in mat4 instanceMatrix;\n"
		"out vec2 TexCoords;\n"
		"out vec4 Color;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"	//位置\n"
		"	vec3 pos = vertex + instanceMatrix[0].xyz;\n"
		"	pos *= instanceMatrix[0].w;\n"
		"	gl_Position = projection * vec4(pos, 1.0f);\n"
		"	//颜色\n"
		"	Color = instanceMatrix[1];\n"
		"	TexCoords.x = texcoord.x + 0.091f*instanceMatrix[2].x;\n"
		"	TexCoords.y = texcoord.y + 0.167f*instanceMatrix[2].y;\n"
		"}\n";

	std::string fShader =
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"in vec4 Color;\n"
		"out vec4 color;\n"
		"uniform sampler2D image;\n"
		"void main() {\n"
		"	vec4 tex = texture(image, TexCoords);\n"
		"	color.xyz = Color.xyz;\n"
		"	color.w = (1.0 - tex.x)*Color.w;\n"
		"}\n";
	
	ResourceManager::LoadShaderFromString(vShader,
		fShader,  "sprite");
	/*ResourceManager::LoadShader("Sprite.vert",
		"Sprite.frag", nullptr, "sprite");*/
	// 加载纹理
	ResourceManager::LoadTexture("C:/Windows/number.png", GL_TRUE, "number");
	// 配置着色器
	glm::mat4 projection = glm::perspective(glm::radians(fovy), aspect, znear, zfar);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	//50个字符链
	for (auto x = 0; x < 100; ++x) {
		matrix.push_back(new CharList(znear, zfar, aspect, fovy));
	}
	//渲染精灵，采用实例化渲染
	sprite = std::make_shared<SpriteRenderer>(ResourceManager::GetShader("sprite"), 0.167f, 0.091f);
}

//输入
void Saver::ProcessInput(GLfloat dt) {
	//nothing to do here
}

//更新
void Saver::Update(GLfloat dt) {
	timeCounter += dt;
	//移动
	if (timeCounter >= 0.10f) {
		for (auto it = matrix.begin(); it != matrix.end(); ++it) {
			(*it)->Move(dt);
		}
		timeCounter = 0;
		//检查是否飘出屏幕外
		for (auto it = matrix.begin(); it != matrix.end(); ++it) {
			if ((*it)->IsOufScreen()) {
				CharList* tmp = *it;
				*it = new CharList(znear, zfar, aspect, fovy);
				delete tmp;
			}
		}
	}
	//传入sprite实例化
	sprite->SetInstance(matrix);
}

//渲染
void Saver::Render() {
	sprite->DrawSprite(ResourceManager::GetTexture("number"));
}