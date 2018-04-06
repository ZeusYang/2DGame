#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount, float s)
	: shader(shader), texture(texture), amount(amount), scale(s)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset, int way)
{
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();//找到第一个未使用的粒子位置
		this->respawnParticle(this->particles[unusedParticle], object, offset, way);
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5;
		}
	}
}

// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.Use();
	for (auto it = particles.begin();it != particles.end();++it)
	{
		if (it->Life > 0.0f)
		{
			this->shader.SetVector2f("offset", it->Position);
			this->shader.SetVector4f("color", it->Color);
			this->shader.SetFloat("scale", scale);
			this->texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	this->particles.resize(this->amount, Particle());
	//for (GLuint i = 0; i < this->amount; ++i)
	//	this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search，在Last之后未找到有被杀死的粒子
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one
	//(note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset,int way)
{
	//随机产生一个粒子
	//-5到+5的随机数
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	//随机颜色
	GLfloat rColor1 = 0.5 + ((rand() % 100) / 100.0f);
	GLfloat rColor2 = -1.0 + ((rand() % 100) / 100.0f)*2;
	GLfloat rColor3 = -1.0 + ((rand() % 100) / 100.0f)*2;
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor1, (rColor2 + 1.0f)/2.0f, (rColor3 + 1.0f)/2.0f, 1.0f);
	particle.Life = 1.0f;
	if(way == 1)particle.Velocity = object.Velocity * 0.2f;
	else if (way == 2)particle.Velocity = glm::length(glm::vec2(150,-450)) * 0.3f * glm::vec2(rColor2, rColor3);
}

void ParticleGenerator::Reset()
{
	for (auto &particle : this->particles) {
		particle.Life = 0.0f;
	}
}