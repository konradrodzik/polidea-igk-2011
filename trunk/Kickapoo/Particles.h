#pragma once
#include "Common.h"

struct particle_vertex
{
	D3DXVECTOR3 pos;
	float size;
	D3DCOLOR color;	
};

struct Properties
{
	D3DXVECTOR3 start;
	D3DXVECTOR3 dirVec;
	bool looping;
	float time;
	float lifeTime;
	float velocity;
	float size;
	int type;
	bool needRotation;
	float respawnTime;
};

class ParticleEffect
{
public:
	ParticleEffect() : vert(), prop()
	{
	}

	//! return false when is dead
	virtual bool update() = 0;
	void draw(VertexBuffer* vb, BufferChunk& chunk);

	Texture texture;
	std::vector<particle_vertex> vert;
	std::vector<Properties> prop;
};

class ParticleSystem : public Singleton<ParticleSystem>
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	void create()
	{
		vb = new VertexBuffer(32768 * sizeof(vertex));	// 2^15
		vb->create();
	}

	void spawn(ParticleEffect* effect)
	{
		instances.push_back(effect);
	}

	void updateParticles();
	void renderParticles();

	void clear();

private:
	VertexBuffer* vb;
	BufferChunk chunk;

	std::vector<ParticleEffect*> instances;
};

DefineAccessToSingleton(ParticleSystem)
