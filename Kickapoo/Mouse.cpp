#include "Common.h"
#include "Mouse.h"

Mouse::Mouse(void)
{
	size = 16.0f;
}

Mouse::~Mouse(void)
{
}

void Mouse::create()
{
	cursor = "cursor.png";
//	x = width / 2;
//	y = height / 2;
}

void Mouse::update()
{
/*	float sensitivity = 300.0f;
	int dx, dy;
	g_Input()->getMovement(dx, dy);

	x += dx * sensitivity * g_Timer()->getFrameTime(); 
	y -= dy * sensitivity * g_Timer()->getFrameTime();

	if(x < 0) x = 0; if(x > width) x = width;
	if(y < 0) y = 0; if(y > height) y = height;
*/
}

void Mouse::drawCursor()
{
//	cursor.set();
//	g_Renderer()->setIdentity();
//	g_Renderer()->drawRect(x - size/2, y - size/2, size, size);
}