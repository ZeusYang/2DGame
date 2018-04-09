#include "Game.h"
#include "CollisionDetect.h"
#include "ResourceManager.h"
#include "Menu.h"
#include <iostream>
#include <sstream>

Game::Game(GLuint width, GLuint height)
	:Width(width),Height(height),State(GAME_MENU), unitX(30), unitY(30),
	gridX(Width /unitX),gridY(Height/unitY),timer(0.0f), firetimer(0.0f)
	,mode(1),score(0)
{
	InitVelocity = glm::vec2(+unitX, +0);
	algorithm = std::make_shared<Algorithm>(this->gridY, this->gridX);
}

Game::~Game()
{
}

//游戏初始化
void Game::Init() {
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

	//游戏控件
	sound = std::shared_ptr<ISoundEngine>(createIrrKlangDevice());
	sprite = std::make_shared<SpriteRenderer>(ResourceManager::GetShader("sprite"));

	// 加载纹理
	ResourceManager::LoadTexture("../res/Textures/background.png", GL_FALSE, "background");
	ResourceManager::LoadTexture("../res/Textures/particle.bmp", GL_FALSE, "snakebody");
	ResourceManager::LoadTexture("../res/Textures/Light.png", GL_FALSE, "particle");

	//蛇
	glm::vec2 headPos = glm::vec2(
		(gridX - 1) / 2 * unitX, (gridY - 1) / 2 * unitY
	);
	snake = std::make_shared<SnakeObject>(headPos, unitX / 2, InitVelocity,
		ResourceManager::GetTexture("snakebody"));

	//食物
	food = std::make_shared<GameObject>(glm::vec2(0.0, 0.0), glm::vec2(unitX, unitY),
		ResourceManager::GetTexture("snakebody"));
	SetFoodPos();
	food->Velocity = glm::vec2(300, 300);
	firework = std::make_shared<GameObject>(glm::vec2(0.0, 0.0), glm::vec2(unitX, unitY),
		ResourceManager::GetTexture("snakebody"));
	firework->Velocity = glm::vec2(200, 200);
	//后处理特效
	effects = std::make_shared<PostProcessor>(ResourceManager::GetShader("postprocessing"),
		this->Width, this->Height);
	//文本显示
	text = std::make_shared<TextRenderer>(this->Width, this->Height);
	text->Load("../res/Fonts/ocraext.TTF", 24);
	//背景音乐
	sound->play2D("../res/Audio/greedysnake.mp3", GL_TRUE);
	//粒子特效
	temptation = std::make_shared<ParticleGenerator>(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		800,//粒子数量
		unitX,//粒子大小
		1.5f,//粒子最终寿命
		1.0/1.5f//透明度衰减速度
		);
	boom = std::make_shared<ParticleGenerator>(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		410,//粒子数量
		unitX*1.5,//粒子大小
		2.0f,//粒子最终寿命
		1.0f/2.0f//透明度衰减速度
		);
}

//处理输入
void Game::ProcessInput(GLfloat dt) {
	if (this->State == GAME_ACTIVE && this->mode == 1) {//玩家模式才能移动
		// 移动挡板
		if (this->Keys[GLFW_KEY_LEFT]) {//左移
			if (snake->Velocity != glm::vec2(+unitX, 0)) {//不能往相反的方向走
				snake->nextdir = glm::vec2(-unitX, 0);
				KeysProcessed[GLFW_KEY_LEFT] = GL_TRUE;
			}
		}
		if (this->Keys[GLFW_KEY_RIGHT]) {//右移
			if (snake->Velocity != glm::vec2(-unitX, 0)) {//不能往相反的方向走
				snake->nextdir = glm::vec2(+unitX, 0);
				KeysProcessed[GLFW_KEY_RIGHT] = GL_TRUE;
			}
		}
		if (this->Keys[GLFW_KEY_UP]) {//上移
			if (snake->Velocity != glm::vec2(0, +unitY)) {//不能往相反的方向走
				snake->nextdir = glm::vec2(0, -unitY);
				KeysProcessed[GLFW_KEY_UP] = GL_TRUE;
			}
		}
		if (this->Keys[GLFW_KEY_DOWN]) {//下移
			if (snake->Velocity != glm::vec2(0, -unitY)) {//不能往相反的方向走
				snake->nextdir = glm::vec2(0, +unitY);
				KeysProcessed[GLFW_KEY_DOWN] = GL_TRUE;
			}
		}
	}
	if (this->State == GAME_MENU) {
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {//确定
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			Reset();
		}
		if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1]) {//玩家模式
			this->mode = 1;
			this->KeysProcessed[GLFW_KEY_1] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2]) {//AI模式
			this->mode = 2;
			this->KeysProcessed[GLFW_KEY_2] = GL_TRUE;
		}
	}
	if (this->State == GAME_LOST || this->State == GAME_WIN) {//游戏输赢之后按确定重新游戏
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]){
			this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			this->State = GAME_MENU;
			Reset();
		}
	}
}

//游戏更新
void Game::Update(GLfloat dt) {
	if (this->State == GAME_ACTIVE && this->mode == 1) {//玩家模式
		//蛇身移动
		snake->Move(dt, *algorithm);
		//做碰撞检测
		DoCollisions();
	}
	
	if (this->State == GAME_ACTIVE && this->mode == 2) {//AI模式
		if (algorithm->path.empty()) {//路径为空，说明蛇当前的路径走完了
			std::pair<int, int> src, det;
			//要注意x和y的顺序是相反的
			src.first = snake->GetHeadPos().y / snake->Size.y;
			src.second = snake->GetHeadPos().x / snake->Size.x;
			det.first = food->Position.y / food->Size.y;
			det.second = food->Position.x / food->Size.x;
			if (!algorithm->Brute_force(src, det)) {
				//无路可走，失败
				this->State = GAME_LOST;
				
			}
		}
		//有路可走
		if (!algorithm->path.empty()) {
			//从路径里取出一步
			std::pair<int, int> next = algorithm->path.front();
			algorithm->path.pop_front();
			//std::cout << next.first << "," << next.second << std::endl;
			glm::vec2 step = glm::vec2(next.second*snake->Size.x, next.first*snake->Size.y);
			snake->MoveByAi(step, *algorithm);
		}
		//做碰撞检测
		DoCollisions();
	}

	//粒子更新
	temptation->Update(dt, *food, 8, food->Size / 20.f, 2);
	//烟花爆炸
	if (firetimer > 0) {
		firetimer -= dt;
		boom->Update(dt, *firework, 0, firework->Size / 2.0f, 3);
	}
	else if (State != GAME_ACTIVE) {//非激活状态随机位置绽放烟花
		firetimer = 2.0f;
		//随机位置
		int x1 = rand() % gridX;
		int y1 = rand() % gridY;
		firework->Position = glm::vec2(x1*unitX, y1*unitY);
		boom->Reset();
		boom->Update(0.f, *firework, 400, firework->Size / 2.0f, 3);
	}
}

//渲染
void Game::Render() {
	std::stringstream ss; ss << this->score;
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN || this->State == GAME_LOST) {
		// 绘制背景
		sprite->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//蛇身
		snake->Draw(*sprite);
		//食物
		food->Draw(*sprite);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//粒子
		temptation->Draw();
		//烟花爆炸
		if(firetimer>0)boom->Draw();
		//分数
		text->RenderText("Score:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU) {
		Menu::Selection(*text, this->Width, this->Height);
	}
	if (this->State == GAME_WIN) {
		Menu::Win(*text, this->Width, this->Height);
	}
	if(this->State == GAME_LOST){
		//分数
		Menu::Lost(*text, this->Width, this->Height, ss.str());
	}
}

//碰撞检测
void Game::DoCollisions() {
	if (State == GAME_ACTIVE) {
		//贪吃蛇吃到食物
		if (CollisionDetect::CheckCollision2(*snake, *food)) {
			snake->AddBody(food->Position);//增加一节蛇身
			//发射爆炸烟
			firetimer = 2.0f;
			firework->Position = food->Position;
			boom->Reset();
			boom->Update(0.f, *firework, 400, firework->Size / 2.0f, 3);
			SetFoodPos();//重新生成食物
			sound->play2D("../res/Audio/get.wav", GL_FALSE);
			//获取一分
			++score;
		}

		//撞到墙壁
		if (CollisionDetect::CheckCollisionWidthWall(*snake, this->Width, this->Height)) {
			this->State = GAME_LOST;
			sound->play2D("../res/Audio/dead.mp3", GL_FALSE);
		}
		
		//自己吃自己
		/*if (snake->isCollisionSelf()) {
			this->State = GAME_LOST;
			sound->play2D("../res/Audio/dead.mp3", GL_FALSE);
		}*/
	}
}

//设置食物位置
void Game::SetFoodPos() {
	int x1 = rand() % gridX;
	int y1 = rand() % gridY;
	while (!algorithm->CouldPlaceFood(x1, y1)) {
		x1 = rand() % gridX;
		y1 = rand() % gridY;
		//std::cout << "??" << std::endl;
	}
	food->Position = glm::vec2(x1*unitX, y1*unitY);
}

//重置至初始状态
void Game::Reset(){
	timer = 0.0f;
	firetimer = 0.0f;
	score = 0;
	glm::vec2 headPos = glm::vec2(
		(gridX - 1) / 2 * unitX, (gridY - 1) / 2 * unitY
	);
	snake->Reset(headPos, InitVelocity);
}