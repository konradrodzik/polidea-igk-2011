#include "Common.h"
#include "ParticleEmitter.h"
#include <math.h>

ParcitleEmitter::ParcitleEmitter()
{
}

ParcitleEmitter::~ParcitleEmitter()
{

}

const float KMaxAtOnce = 200;

void ParcitleEmitter::Emit(b2Vec2 pos, float curTime, int count, float dirX, float dirY)
{
	float dt = 0.0f;
	for(int i = 0 ; i < count ; ++i)
	{
		Particle p;

		if(i != 0 && (i % (int)(KMaxAtOnce)) == 0)
			dt += 1.0f;

		p.startTime = curTime + dt + RandomFloat(-0.5f, 0.5f);
		p.endTime = p.startTime + RandomFloat(0.0f, 0.5f);
		p.pos = pos;
		DirY = dirY;

		p.pos.x += RandomFloat(-0.5f, 0.5f);
		p.pos.y += RandomFloat(0.0f, 2.0f);
		if(dirX == 0.0f)
			p.vec.x = RandomFloat(-3.0f,3.0f);
		else
			p.vec.x = dirX;

		m_Items.push_back(p);
	}
}

void ParcitleEmitter::Render(b2Vec2 pos, float curTime)
{
	for(std::list<Particle>::iterator it = m_Items.begin() ; it != m_Items.end() ; ++it)
	{
		if(it->startTime > curTime)
			continue;
		g_Renderer()->drawLine(
			it->pos.x + pos.x,
			it->pos.y + pos.y,
			it->pos.x + pos.x + 0.2f,
			it->pos.y + pos.y+0.1f,
			0.1f);
	}
}

void ParcitleEmitter::Update(float curTime)
{
	std::list<Particle>::iterator next_it;
	int lost = 0;

	for(std::list<Particle>::iterator it = m_Items.begin() ; it != m_Items.end() ; it = next_it )
	{
		next_it = it;
		next_it++;
		if(curTime < it->startTime)
			continue;
		if(curTime > it->endTime)
		{
			m_Items.erase(it);
			lost++;
			continue;
		}

		//g = g_World()->getGravitySign() * 9.81f;

		//it->pos.x += it->vec.x * g_Timer()->getFrameTime();
		//it->pos.y += g_Timer()->getFrameTime() * g * DirY;
	}
}
