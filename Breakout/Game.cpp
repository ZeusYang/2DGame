#include <irrKlang.h>
using namespace irrklang;
#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "BallObject.h"
#include "TextRenderer.h"

// Game-related State data
SpriteRenderer  *Renderer;

// 初始化挡板的大小
const glm::vec2 PLAYER_SIZE(200, 40);
// 初始化挡板的速率
const GLfloat PLAYER_VELOCITY(500.0f);
//玩家挡板
GameObject      *Player;

// 初始化球的速度
const glm::vec2 INITIAL_BALL_VELOCITY(150.0f, -450.0f);
// 球的半径
const GLfloat BALL_RADIUS = 25.0f;
//运动的小球
BallObject     *Ball;

//粒子生成器
ParticleGenerator   *Particles;

//后期特效处理
PostProcessor   *Effects;
GLfloat ShakeTime = 0.0f;

//音效
ISoundEngine *SoundEngine = createIrrKlangDevice();
//文本显示
TextRenderer  *Text;

Game::Game(GLuint width, GLuint height)
	:State(GAME_MENU), Keys(), Width(width), Height(height)
{
}

Game::~Game(){
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	SoundEngine->drop();
}

void Game::Init(){
	// 加载着色器
	ResourceManager::LoadShader("../res/Shaders/Sprite.vert", 
		"../res/Shaders/Sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("../res/Shaders/Particle.vert",
		"../res/Shaders/Particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("../res/Shaders/Effect.vert", 
		"../res/Shaders/Effect.frag", nullptr, "postprocessing");

	// 配置着色器
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	// 设置专用于渲染的控制
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("../res/Textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("../res/Textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("../res/Textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::LoadTexture("../res/Textures/particle.png", GL_TRUE, "particle");
	ResourceManager::LoadTexture("../res/Textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::LoadTexture("../res/Textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::LoadTexture("../res/Textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::LoadTexture("../res/Textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::LoadTexture("../res/Textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
	ResourceManager::LoadTexture("../res/Textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");

	// 加载关卡
	GameLevel one; one.Load("../res/levels/one.lvl", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("../res/levels/two.lvl", this->Width, this->Height * 0.5);
	GameLevel three; three.Load("../res/levels/three.lvl", this->Width, this->Height * 0.5);
	GameLevel four; four.Load("../res/levels/four.lvl", this->Width, this->Height * 0.5);
	this->Levels.push_back(one);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 1;

	//玩家
	glm::vec2 playerPos = glm::vec2(
		this->Width / 2 - PLAYER_SIZE.x / 2,
		this->Height - PLAYER_SIZE.y
	);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	//小球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));

	//粒子效果
	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500
	);

	//后处理特效
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
	//文本显示
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("../res/Fonts/ocraext.TTF", 24);
	//背景音乐
	SoundEngine->play2D("../res/Audio/breakout.mp3", GL_TRUE);
	Lives = 3;
}

void Game::Update(GLfloat dt){
	 Ball->Move(dt, this->Width);
	 this->DoCollisions();
	 // Update particles
	 Particles->Update(dt, *Ball, 4, glm::vec2(Ball->Radius / 2));
	 // Update PowerUps
	 this->UpdatePowerUps(dt);
	 if (Ball->Position.y >= this->Height) // 球是否接触底部边界？
	 {
		 --this->Lives;
		 // 玩家是否已失去所有生命值? : 游戏结束
		 if (this->Lives == 0){
			 this->ResetLevel();
			 this->State = GAME_MENU;
		 }
		 this->ResetPlayer();
	 }
	 if (ShakeTime) {
		 ShakeTime -= dt;
		 if (ShakeTime < 0.0f)Effects->Shake = false;
	 }
	 //获胜
	 if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
	 {
		 this->ResetLevel();
		 this->ResetPlayer();
		 Effects->Chaos = GL_TRUE;
		 this->State = GAME_WIN;
	 }
}

void Game::ProcessInput(GLfloat dt){
	if (this->State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// 移动挡板
		if (this->Keys[GLFW_KEY_A]) {//左移
			if (Player->Position.x >= 0)
				Player->Position.x -= velocity;
			if (Ball->Stuck)//小球随着挡板移动
				Ball->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D]) {//右移
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
			if (Ball->Stuck)//小球随着挡板移动
				Ball->Position.x += velocity;
		}
		if (this->Keys[GLFW_KEY_SPACE])//释放小球
			Ball->Stuck = false;
	}
	if (this->State == GAME_MENU)
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			if (this->Level >= 1)
				--this->Level;
			else
				this->Level = 4;
			this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}
	if (this->State == GAME_WIN)
	{
		if (this->Keys[GLFW_KEY_ENTER])
		{
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			Effects->Chaos = GL_FALSE;
			this->State = GAME_MENU;
		}
	}
}

void Game::Render(){
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN){
		Effects->BeginRender();
		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// 绘制关卡
		this->Levels[this->Level].Draw(*Renderer);
		//玩家挡板和小球
		Player->Draw(*Renderer);
		// Draw PowerUps
		for (PowerUp &powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		// Draw particles   
		Particles->Draw();
		// Draw ball
		Ball->Draw(*Renderer);
		//后期处理
		Effects->EndRender();
		Effects->Render(glfwGetTime());
		//生命值
		std::stringstream ss; ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU)
	{
		Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
		Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN)
	{
		Text->RenderText(
			"You WON!!!", 420.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0)
		);
		Text->RenderText(
			"Press ENTER to retry or ESC to quit", 230.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
		);
	}
}

GLboolean Game::CheckCollisionAABB(GameObject &one, GameObject &two)//AABB碰撞检测
{	//我们检查第一个物体的最右侧是否大于第二个物体的最左侧并且第二个物体的最右侧是否大于第一个物体的最左侧；
	//垂直的轴向与此相似。
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x
		&& two.Position.x + two.Size.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y
		&& two.Position.y + two.Size.y >= one.Position.y;
	return collisionX && collisionY;
}

Collision  Game::CheckCollisionSphere(BallObject &one, GameObject &two){//包围球碰撞检测
	// 获取圆的中心 
	glm::vec2 center(one.Position + one.Radius);//Position是左上角
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	difference = closest - center;
	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

void ActivatePowerUp(PowerUp &powerUp)
{
	// 根据道具类型发动道具
	if (powerUp.Type == "speed")
	{
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ball->Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ball->PassThrough = GL_TRUE;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = GL_TRUE; // 只在chaos未激活时生效，chaos同理
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = GL_TRUE;
	}
}

void Game::DoCollisions() {//检测碰撞
	for (GameObject &box : this->Levels[this->Level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollisionSphere(*Ball, box);
			if (std::get<0>(collision)) // 如果collision 是 true
			{
				// 如果砖块不是实心就销毁砖块
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
					SoundEngine->play2D("../res/Audio/bleep.mp3", GL_FALSE);
				}
				else {
					ShakeTime = 0.10f;
					Effects->Shake = true;
					SoundEngine->play2D("../res/Audio/solid.wav", GL_FALSE);
				}
				// 碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !box.IsSolid)) {
					if (dir == LEFT || dir == RIGHT) // 水平方向碰撞
					{
						Ball->Velocity.x = -Ball->Velocity.x; // 反转水平速度
															  // 重定位
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration; // 将球右移
						else
							Ball->Position.x -= penetration; // 将球左移
					}
					else // 垂直方向碰撞
					{
						Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
															  // 重定位
						GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration; // 将球上移
						else
							Ball->Position.y += penetration; // 将球下移
					}
				}
			}
		}
	}
	//玩家碰撞
	Collision result = CheckCollisionSphere(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		// 依据结果移动
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
		Ball->Stuck = Ball->Sticky;
		SoundEngine->play2D("../res/Audio/bleep.wav", GL_FALSE);
	}

	for (PowerUp &powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)//超出屏幕摧毁
				powerUp.Destroyed = GL_TRUE;
			if (CheckCollisionAABB(*Player, powerUp)){   // 道具与挡板接触，激活它！
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
				SoundEngine->play2D("../res/Audio/powerup.wav", GL_FALSE);
			}
		}
	}
}

Direction  Game::VectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),  // 上
		glm::vec2(1.0f, 0.0f),  // 右
		glm::vec2(0.0f, -1.0f), // 下
		glm::vec2(-1.0f, 0.0f)  // 左
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++){
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max){
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::ResetLevel() {
	if (this->Level == 1)this->Levels[0].Load("../res/levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[1].Load("../res/levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[2].Load("../res/levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 4)
		this->Levels[3].Load("../res/levels/four.lvl", this->Width, this->Height * 0.5f);
	this->Lives = 3;
}

void Game::ResetPlayer() {
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

GLboolean ShouldSpawn(GLuint chance)
{
	GLuint random = rand() % chance;
	return random == 0;
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type){
	// Check if another PowerUp of the same type is still active
	// in which case we don't disable its effect (yet)
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}

void Game::UpdatePowerUps(GLfloat dt)
{
	for (PowerUp &powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// 之后会将这个道具移除
				powerUp.Activated = GL_FALSE;
				// 停用效果
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
						Ball->Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{
						Ball->PassThrough = GL_FALSE;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{
						Effects->Confuse = GL_FALSE;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{
						Effects->Chaos = GL_FALSE;
					}
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

void Game::SpawnPowerUps(GameObject &block)
{
	if (ShouldSpawn(75)) // 1 in 75 chance
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f,
			block.Position, ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
			block.Position, ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 
			10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4),
			0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // Negative powerups should spawn more often
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f,
			block.Position, ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f,
			block.Position, ResourceManager::GetTexture("powerup_chaos")));
}