#ifndef __ROUTE_PLANER__
#define __ROUTE_PLANER__
#include "Common.h"
#include "Map.h"

#define buttonStartX  20
#define buttonStartY  250
#define buttonSize  64
#define buttonMarginX  10
#define buttonMarginY  10

#define mapScale 8

class RoutePlaner {
public:
	struct Button {
		RECT rect;
		Group *group;
		Vehicle *vehicle;

		BOOL mouseInside() {
			if (rect.left > Mouse::getSingletonPtr()->getX() || rect.right < Mouse::getSingletonPtr()->getX()) return FALSE;
			if (rect.top > Mouse::getSingletonPtr()->getY() || rect.bottom < Mouse::getSingletonPtr()->getY()) return FALSE;
			return TRUE;
		}
	};

	Button *buttons[MaxMapGroups * 4];
	unsigned int buttonCount;
	Button *lastClickedButton;
	unsigned int lastClickedButtonIndex;
	Game *game_;
	Texture startGame;
	Group *selectedGroup;
	int mapX;
	int mapY;
	int mapHeight;
	int mapWidth;
public:
	RoutePlaner();
	void placeButtons();
	void update();
	void draw();
};


#endif