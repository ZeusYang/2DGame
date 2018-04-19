#include "SpriteRenderer.h"
#include <iostream>

SpriteRenderer::SpriteRenderer(Shader &shader, float unitX, float unitY)
	:shader(shader), amount(0)
{
	this->initRenderData(unitX, unitY);
}

SpriteRenderer::~SpriteRenderer() {
	glDeleteVertexArrays(1, &this->quadVAO);
	glDeleteBuffers(1, &this->instanceVBO);
}

void SpriteRenderer::initRenderData(float unitX,float unitY)
{
	// 配置 VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// 位置				// 纹理
		-0.5f, 0.5f,0.0f, 0.000f,  unitX,
		 0.5f,-0.5f,0.0f,  unitY, 0.000f,
		-0.5f,-0.5f,0.0f, 0.000f, 0.000f,

		-0.5f, 0.5f,0.0f, 0.000f,  unitX,
		 0.5f, 0.5f,0.0f,  unitY,  unitX,
		 0.5f,-0.5f,0.0f,  unitY, 0.000f
	};
	
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &instanceVBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::SetInstance(const std::vector<CharList*> &target) {
	//获取实例矩阵
	std::vector<glm::mat4> instance;
	amount = target.size()*target[0]->necklace.size();
	instance.reserve(amount);
	for (auto it = target.begin(); it != target.end(); ++it) {
		for (auto th = (*it)->necklace.begin(); th != (*it)->necklace.end(); ++th) {
			glm::mat4 tmp(1.0f);
			tmp[0] = th->Pos;
			tmp[1] = th->Color;
			tmp[2] = th->Coord;
			tmp[3] = glm::vec4(1.0f);
			instance.push_back(tmp);
		}
	}

	GLsizei vec4Size = sizeof(glm::vec4);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instance.size() * sizeof(glm::mat4), &instance[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec2 position,
	glm::vec2 size, GLfloat rotate, glm::vec3 color){
	this->shader.Use();
	//绑定纹理
	glActiveTexture(GL_TEXTURE0);
	texture.Bind();
	glBindVertexArray(this->quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);
	glBindVertexArray(0);
}