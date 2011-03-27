#pragma once
#include "Common.h"

struct WaterIndex
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 uv;

	float time;
	float range;
	float dir;

	void set(float x, float y, float u, float v) 
	{
		pos.x = x;
		pos.y = y;
		uv.x  = u;
		uv.y  = v;

		time	= RandomFloat(0, D3DX_PI);
		range	= RandomFloat(-0.015f, 0.015f);
		dir		= /*(range > 0) ? -1 : */1;
	}

	void simulete(float dt)
	{
		float sinValue, cosValue;

		time += dt*4;
		if (time > 10000) {
			time = 0;
		}

		sinValue = sin(time) * range * dt * dir;
		range = RandomFloat(0, 0.006f);
		cosValue = cos(time) * range * dt * dir * 2;

		//uv.x  += sinValue; //* tan(RandomFloat(RandomFloat(0, 1), RandomFloat(1, 2)));
		uv.y  -= cosValue; //* tan(RandomFloat(RandomFloat(0, 1), RandomFloat(1, 2)));
	}
};

class Water
{
public:
	Water(void)		{}
	~Water(void)	{}

	void create(float x, float y, float width, float height, int waterDensity);
	void release();

	void update();
	void render();
		
	void setTexture(LPDIRECT3DTEXTURE9* texture_) { 
		tex = texture_; 
	}

private:
	LPDIRECT3DTEXTURE9* tex;
	WaterIndex* waterIndex;
	int WATER_DENSITY;
	
	void renderData(int x, int y);
};
