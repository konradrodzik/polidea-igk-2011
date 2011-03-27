#pragma once
#include "Common.h"

class Mouse : public Singleton<Mouse>
{
	float x, y;
	float size;
	Texture cursor;

public:
	Mouse(void);
	~Mouse(void);

	void create();

	void update();
	void drawCursor();
	void setPos(float x_, float y_)
	{
		x = x_;
		y = y_;
	}

	float getX() { return x; }
	float getY() { return y; }
};
DefineAccessToSingleton(Mouse);