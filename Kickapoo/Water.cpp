#include "Common.h"
#include "Water.h"

void Water::create(float x_, float y_, float width, float height, int waterDensity)
{
	float stepX, stepY;
	float stepCoordX, stepCoordY;
	float moveCoordX, moveCoordY;

	WATER_DENSITY = waterDensity;

	waterIndex = new WaterIndex[WATER_DENSITY * WATER_DENSITY];

	stepX = width  / float(WATER_DENSITY - 1);
	stepY = height / float(WATER_DENSITY - 1);

	stepCoordX = 1.0f / float(WATER_DENSITY - 1);//1024.0f;	// 1.0f / float(WATER_DENSITY - 1) * 
	stepCoordY = 1.0f / float(WATER_DENSITY - 1);//1024.0f; // 1.0f / float(WATER_DENSITY - 1) * 

	moveCoordX = x_ / 1024.0f; 
	moveCoordY = y_ / 1024.0f;

	for (int x = 0; x < WATER_DENSITY; x++)
	{
		for (int y = 0; y < WATER_DENSITY; y++)
		{
			waterIndex[x * WATER_DENSITY + y].set(x_ + x * stepX, y_ + y * stepY, 
				x * stepCoordX, (WATER_DENSITY - 1 - y) * stepCoordY);
		}
	}
}

void Water::release()
{
	delete waterIndex;
}

void Water::update()
{
	for (int x = 1; x < WATER_DENSITY - 1; x++)
	{
		for (int y = 1; y < WATER_DENSITY - 1; y++)
		{
			waterIndex[x * WATER_DENSITY + y].simulete( g_Timer()->getFrameTime() + 
				((x+y)%2) * g_Timer()->getFrameTime() );
		}
	}
}

void Water::renderData(int x, int y)
{
	g_Renderer()->drawRect(
		waterIndex[x * WATER_DENSITY + y].pos.x,			waterIndex[x * WATER_DENSITY + y].pos.y,			waterIndex[x * WATER_DENSITY + y].uv.x,			waterIndex[x * WATER_DENSITY + y].uv.y,
		waterIndex[(x+1) * WATER_DENSITY + y].pos.x,		waterIndex[(x+1) * WATER_DENSITY + y].pos.y,		waterIndex[(x+1) * WATER_DENSITY + y].uv.x,		waterIndex[(x+1) * WATER_DENSITY + y].uv.y,
		waterIndex[(x+1) * WATER_DENSITY + y + 1].pos.x,	waterIndex[(x+1) * WATER_DENSITY + y + 1].pos.y,	waterIndex[(x+1) * WATER_DENSITY + y + 1].uv.x,	waterIndex[(x+1) * WATER_DENSITY + y + 1].uv.y,
		waterIndex[x * WATER_DENSITY + y + 1].pos.x,		waterIndex[x * WATER_DENSITY + y + 1].pos.y,		waterIndex[x * WATER_DENSITY + y + 1].uv.x,		waterIndex[x * WATER_DENSITY + y + 1].uv.y
		);
}

void Water::render()
{
	getDevice()->SetTexture(0, *tex);
	for (int x = 0; x < WATER_DENSITY - 1; x++)
	{
		for (int y = 0; y < WATER_DENSITY - 1; y++)
		{
			renderData(x, y);
		}
	}
}
