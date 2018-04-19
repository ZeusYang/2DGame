#pragma once
#include <list>
#include <glm/glm.hpp>

struct Symbol {//单个字符
	glm::vec4 Pos;//位置、大小
	glm::vec4 Color;//颜色
	glm::vec4 Coord;//纹理坐标的移动
};

#define M_PI 3.1415926535

class CharList{
public:
	CharList(float zn, float zf, float ap, float fy);

	~CharList() = default;

	void Move(float dt);//移动

	bool IsOufScreen();//判断是否飘出屏幕外
	
	std::list<Symbol> necklace;

	glm::vec3 pos;//列尾位置
	glm::vec3 vel;//速度方向

	int num;//字符数量
	
private:
	float size, znear, zfar, aspect, fovy;
	
	void RndPos();//随机位置

	inline float Rand0_1() {//0-1的随机数
		return (double)rand() / (double)RAND_MAX;
	}
};

