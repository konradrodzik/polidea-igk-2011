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
	v.pos = pos + D3DXVECTOR3(RandomFloat(-5, 5), RandomFloat(-5, 5), 0);
	
	float c = RandomFloat(0.2, 1);
	v.color = D3DXCOLOR(c, c, c, 1);
	v.size = RandomFloat(2, 3);

	Properties p;
	p.lifeTime = 1;
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
	if(elapsedTime > 0.01f && enabled)
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

Nova::Nova(D3DXVECTOR3 p, float time, float velocity)
{
	t = time;
	v_ = velocity;

	elapsedTime = 0;
	enabled = true;

	pos = p;

	texture = "smoketex.jpg";
	float c = RandomFloat(0.2, 1);
	for(int i  = 0; i < 500; ++i)
	{
		spawn(D3DXCOLOR(c, c, c, 1));
	}
}

Nova::~Nova(void)
{
}

void Nova::spawn(D3DCOLOR c)
{
	particle_vertex v;
	v.pos = pos;
	
	v.color = c;
	v.size = RandomFloat(2, 3);

	Properties p;
	p.lifeTime = t;
	p.time = 0;
	p.dirVec = D3DXVECTOR3(RandomFloat(-1, 1), RandomFloat(0, 1), RandomFloat(-1, 1));
	D3DXVec3Normalize(&p.dirVec, &p.dirVec);
	p.velocity = v_;

	vert.push_back(v);
	prop.push_back(p);
}

bool Nova::update()
{
	if(vert.empty()) {
		return false;
	}

	for(int i = 0; i < vert.size(); )
	{
		vert[i].pos += prop[i].dirVec * prop[i].velocity * g_Timer()->getFrameTime();

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

Fire::Fire(D3DXVECTOR3 p)
{
	elapsedTime = 0;
	enabled = true;

	pos = p;

	texture = "smoketex.jpg";
	for(int i  = 0; i < 500; ++i)
	{
		spawn();
	}
}

Fire::~Fire(void)
{
}

void Fire::spawn()
{
	particle_vertex v;
	v.pos = pos + D3DXVECTOR3(RandomFloat(-10, 10), RandomFloat(-10, 10), 0);
	
	float c = RandomFloat(0.2, 1);
	v.color = D3DXCOLOR(RandomFloat(0.2, 1), 0, 0, 1);
	v.size = RandomFloat(2, 3);

	Properties p;
	p.lifeTime = 1;
	p.time = 0;
	p.dirVec = D3DXVECTOR3(RandomFloat(-0.5f, 0.5f), RandomFloat(-0.5f, 0.5f), RandomFloat(0, 10));
	D3DXVec3Normalize(&p.dirVec, &p.dirVec);
	p.velocity = 100;

	vert.push_back(v);
	prop.push_back(p);
}

bool Fire::update()
{
	if(vert.empty()) {
		return false;
	}

	elapsedTime += g_Timer()->getFrameTime();
	if(elapsedTime > 0.01f && enabled)
	{
		spawn();
		elapsedTime = 0;
	}

	for(int i = 0; i < vert.size(); )
	{
		vert[i].pos += prop[i].dirVec * prop[i].velocity * g_Timer()->getFrameTime();

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

void Fire::draw(VertexBuffer* vb, BufferChunk& chunk)
{
	float fPointSize = 0.1f;
	getDevice()->SetRenderState( D3DRS_POINTSIZE, *((DWORD*)&fPointSize) );
	ParticleEffect::draw(vb, chunk);
}
