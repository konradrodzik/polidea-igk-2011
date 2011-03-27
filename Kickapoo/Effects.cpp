#include "Common.h"
#include "Effects.h"

Smoke::Smoke(void)
{
	elapsedTime = 0;
	enabled = true;

	pos = D3DXVECTOR3(0, 0, 0);

	texture = "smoketex.jpg";
	spawn();
}

Smoke::~Smoke(void)
{
}

void Smoke::spawn()
{
	particle_vertex v;
	v.pos = pos + D3DXVECTOR3(RandomFloat(-10, 10), RandomFloat(-10, 10), 0);
	
	float c = RandomFloat(0.2, 1);
	v.color = D3DXCOLOR(c, c, c, 1);
	v.size = RandomFloat(5, 10);

	Properties p;
	p.lifeTime = RandomFloat(3, 5);
	p.time = 0;

	vert.push_back(v);
	prop.push_back(p);
}

bool Smoke::update()
{
	if(vert.empty()) {
		return false;
	}

	elapsedTime += g_Timer()->getFrameTime();
	if(elapsedTime > 0.1f && enabled)
	{
		spawn();
		elapsedTime = 0;
	}

	for(int i = 0; i < vert.size(); )
	{
		prop[i].time += g_Timer()->getFrameTime();
		D3DXCOLOR c(vert[i].color);
		c.a = max(0, 1 - prop[i].time / prop[i].lifeTime);
		vert[i].color = c;
		if(prop[i].time > prop[i].lifeTime)
		{
			vert.erase(vert.begin() + i);
			prop.erase(prop.begin() + i);
		}
		else 
		{
			i++;
		}
	}
	return true;
}

void Smoke::draw(VertexBuffer* vb, BufferChunk& chunk)
{
	float fPointSize = 0.1f;
	getDevice()->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&fPointSize) );
	ParticleEffect::draw(vb, chunk);
}

Nova::Nova(void)
{
	elapsedTime = 0;
	enabled = true;

	pos = D3DXVECTOR3(0, 0, 0);

	texture = "smoketex.jpg";
	spawn();
}

Nova::~Nova(void)
{
}

void Nova::spawn()
{
	particle_vertex v;
	v.pos = pos + D3DXVECTOR3(RandomFloat(-10, 10), RandomFloat(-10, 10), 0);
	
	float c = RandomFloat(0.2, 1);
	v.color = D3DXCOLOR(c, c, c, 1);
	v.size = RandomFloat(5, 10);

	Properties p;
	p.lifeTime = RandomFloat(3, 5);
	p.time = 0;
	p.dirVec = D3DXVECTOR3(RandomFloat(-1, 1), RandomFloat(-1, 1), RandomFloat(0, 1));

	vert.push_back(v);
	prop.push_back(p);
}

bool Nova::update()
{
	if(vert.empty()) {
		return false;
	}

	elapsedTime += g_Timer()->getFrameTime();
	if(elapsedTime > 0.1f && enabled)
	{
		spawn();
		elapsedTime = 0;
	}

	for(int i = 0; i < vert.size(); )
	{
		prop[i].time += g_Timer()->getFrameTime();
		D3DXCOLOR c(vert[i].color);
		c.a = max(0, 1 - prop[i].time / prop[i].lifeTime);
		vert[i].color = c;
		if(prop[i].time > prop[i].lifeTime)
		{
			vert.erase(vert.begin() + i);
			prop.erase(prop.begin() + i);
		}
		else 
		{
			i++;
		}
	}
	return true;
}

void Nova::draw(VertexBuffer* vb, BufferChunk& chunk)
{
	float fPointSize = 0.1f;
	getDevice()->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&fPointSize) );
	ParticleEffect::draw(vb, chunk);
}
