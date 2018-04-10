#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ctime>

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"

// Represents a single particle and its state
struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;
	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
public:
	ParticleGenerator() {}
	// Constructor
	ParticleGenerator(Shader shader, Texture2D texture, GLuint amount,
		float scale = 20.0f, float life = 1.0f, float a = 2.5f);
	// Update all particles
	void Update(GLfloat dt, GameObject &object, GLuint newParticles,
		glm::vec2 offset = glm::vec2(0.0f, 0.0f), int way = 1, int type = 0);
	// Render all particles
	void Draw();
	//粒子复位
	void Reset();
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	float scale;
	// Render state
	Shader shader;
	Texture2D texture;
	GLuint VAO;
	//粒子最终寿命
	GLfloat life;
	//alpha衰减速度
	GLfloat a_atten;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, GameObject &object,
		glm::vec2 offset = glm::vec2(0.0f, 0.0f), int way = 1, int type = 0);
};

