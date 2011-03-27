#pragma once
#include "Common.h"

typedef D3DXVECTOR2 b2Vec2;

class Particle
{
public:
	b2Vec2			pos;
	float			startTime;
	float			endTime;
	b2Vec2			vec;
};

class ParcitleEmitter
{
public:
	float g;
	float			DirY;

	ParcitleEmitter();
	~ParcitleEmitter();

	void Emit(b2Vec2 pos, float curTime, int count, float dirX, float dirY);


	void Render(b2Vec2 pos, float curTime);
	void Update(float curTime);
	bool Kill() const { return m_Items.size() == 0; }
private:
	std::list<Particle>		m_Items;
};
