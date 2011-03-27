#pragma once
#include "Common.h"

class Smoke : public ParticleEffect
{
public:
	Smoke(void);
	~Smoke(void);

	virtual bool update();
	virtual void draw(VertexBuffer* vb, BufferChunk& chunk);

	void spawn();

	D3DXVECTOR3 pos;
	float elapsedTime;
	bool enabled;
};

class Nova : public ParticleEffect
{
public:
	Nova(D3DXVECTOR3 p, float time, float velocity);
	~Nova(void);

	virtual bool update();
	virtual void draw(VertexBuffer* vb, BufferChunk& chunk);

	void spawn(D3DCOLOR c);

	D3DXVECTOR3 pos;
	float elapsedTime;
	bool enabled;
	
	float t;
	float v_;
};

class Fire : public ParticleEffect
{
public:
	Fire(D3DXVECTOR3 p);
	~Fire(void);

	virtual bool update();
	virtual void draw(VertexBuffer* vb, BufferChunk& chunk);

	void spawn();

	D3DXVECTOR3 pos;
	float elapsedTime;
	bool enabled;

};