#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Texture2D.h"

class SpriteRenderer
{
public:
	SpriteRenderer(Shader &shader);
	~SpriteRenderer();

	//绘制精灵，指定纹理、位置、大小、旋转度、颜色
	void DrawSprite(Texture2D &texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));
private:
	Shader shader;
	GLuint quadVAO;
	void initRenderData();
};

