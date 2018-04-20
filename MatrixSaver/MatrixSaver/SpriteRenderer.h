#pragma once
#include <vector>
#include "Shader.h"
#include "Texture2D.h"
#include "CharList.h"

class SpriteRenderer{
public:
	SpriteRenderer(Shader &shader, float untiX, float unitY);

	~SpriteRenderer();

	//实例化
	void SetInstance(const std::vector<CharList*> &target);

	 //绘制精灵，除了纹理，其余参数都没用到
	void DrawSprite(Texture2D &texture, glm::vec2 position = glm::vec2(0, 0),
		glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));

private:
	int amount;//实例化的个数
	Shader shader;
	GLuint quadVAO;
	GLuint instanceVBO;
	void initRenderData(float untiX, float unitY);
};

