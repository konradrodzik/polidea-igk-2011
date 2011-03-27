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
	Nova(void);
	~Nova(void);

	virtual bool update();
	virtual void draw(VertexBuffer* vb, BufferChunk& chunk);

	void spawn();

	D3DXVECTOR3 pos;
	float elapsedTime;
	bool enabled;
};