#pragma once
#include "GameObject.h"

//碰撞的方向
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};
//用于圆形和矩形的碰撞检测
//碰撞的返回值，分别是是否碰撞、碰撞方向、圆心和最近点的矢量差
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class CollisionDetect
{
public:

	//矩形和矩形碰撞检测
	static bool CheckCollision(GameObject &one, GameObject &two);

	//矩形和圆形碰撞检测
	static bool CheckColision1(GameObject &one, GameObject &two);

	//圆形和圆形碰撞检测
	static bool CheckCollision2(GameObject &one, GameObject &two);

	//圆形和圆形碰撞检测
	static bool CheckCollision2(glm::vec2 &one, glm::vec2 &size1, glm::vec2 &two, glm::vec2 &size2);

	//与墙壁碰撞检测
	static bool CheckCollisionWidthWall(GameObject &one, GLuint &width, GLuint &height);
private:
	CollisionDetect() {}
};

