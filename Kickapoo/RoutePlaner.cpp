#include "Common.h"

inline RECT rectMake(int left, int top, int right, int bottom) {
	RECT r;
	r.left = left;
	r.right = right;
	r.top = top;
	r.bottom = bottom;
	return r;
}

void RoutePlaner::placeButtons() {
	for (int i=0; i < buttonCount; ++i) {
		delete buttons[i];
		buttons[i] = NULL;
	}

	int buttonY = buttonStartY;
	int buttonX = buttonStartX;
	buttonCount = 0;

	//! for each group
	for (int g = 0; g < game_->map->groupCount; ++g) {

		//! for each button
		for (int i = 0; i < game_->map->vehicleCount; ++i) {
			Vehicle *vehicle = &game_->map->vehicles[i];
			Group *vehicleGroup = vehicle->group;
			
			if (vehicleGroup == &game_->map->groups[g]) {
				buttons[buttonCount] = new Button();
				buttons[buttonCount]->rect = rectMake(buttonX, buttonY, buttonX + buttonSize, buttonY + buttonSize);
				buttons[buttonCount]->group = vehicleGroup;
				buttons[buttonCount]->vehicle = vehicle;

				buttonX += buttonSize + buttonMarginX;
				buttonCount += 1;
			}
		}

		buttonY += buttonSize + buttonMarginY;
		buttonX = buttonStartX;
	}
}
RoutePlaner::RoutePlaner()
	: startGame("start.png")
{
	selectedGroup = NULL;
	lastClickedButton = NULL;
	lastClickedButtonIndex = 0;
	buttonCount = 0;
	memset(buttons, 0, sizeof(buttons));

	game_ = g_Game;
	placeButtons();
}

void RoutePlaner::update()
{
	int clickedButtonIndex = 0;
	Button *clickedButton = NULL;
	bool buttonPressed = Input::getSingletonPtr()->buttonClicked(0);
	if (buttonPressed) {
		for (int i=0; i < buttonCount; ++i) {
			Button *button = buttons[i];
			if (button->mouseInside()) {
				clickedButtonIndex = i;
				clickedButton = button;
				selectedGroup = button->vehicle->group;
				break;
			}
		}
	}

	if (clickedButton) {
		if (lastClickedButton != NULL && lastClickedButton != clickedButton) {
			//! SWAPING GROUPS
			Group *groupA = clickedButton->vehicle->group;
			Group *groupB = lastClickedButton->vehicle->group;

			//! swap groups
			lastClickedButton->vehicle->group = groupA;
			clickedButton->vehicle->group = groupB;
			
			//! swap rect
			RECT tempRect = lastClickedButton->rect;
			lastClickedButton->rect = clickedButton->rect;
			clickedButton->rect = tempRect;

			//! swap in indexes
			buttons[clickedButtonIndex] = buttons[lastClickedButtonIndex];
			buttons[lastClickedButtonIndex] = clickedButton;

			clickedButton = NULL;
			clickedButtonIndex = UINT_MAX;
		}

		lastClickedButton = clickedButton;
		lastClickedButtonIndex = clickedButtonIndex;
	} else {
		//! route select or start
		if (buttonPressed) {
			//! start
			if (Mouse::getSingletonPtr()->getX() > 800 - 146 && Mouse::getSingletonPtr()->getY() > 600-32) {
				game_->changeState(EGameState::Running);
			} else if (selectedGroup) {
			//! route
				if ((Mouse::getSingletonPtr()->getX() > mapX &&
					Mouse::getSingletonPtr()->getX() < mapX + mapWidth) && ( Mouse::getSingletonPtr()->getY() > mapY && Mouse::getSingletonPtr()->getY() < mapY + mapHeight) ) {
					int tilesX = floor((Mouse::getSingletonPtr()->getX() - mapX) / (float)mapScale);
					int tilesY = floor((Mouse::getSingletonPtr()->getY() - mapY) / (float)mapScale);

					printf("tiles %d %d\n", tilesX, tilesY);
					if (selectedGroup->nodes.size() > 0) {
						Node *node = selectedGroup->nodes.back();
						//! find node connected to this one
#define EPS 0.0001f
#define FLT_EQ(x, y) (x > y - EPS && x < y + EPS)

						for (int i = 0; i < node->otherNodes.size(); ++i) {
							printf("otherNodes[%d].pos = %f %f\n", i, node->otherNodes[i]->position.x, node->otherNodes[i]->position.y);
							if (FLT_EQ(node->otherNodes[i]->position.x, tilesX) && FLT_EQ(node->otherNodes[i]->position.y, tilesY)) {
								selectedGroup->nodes.push_back(node->otherNodes[i]);
								printf("pushing node\n");
								break;
							}
						}
					} else {
						printf("THERE SHOULD BE FIRST NODE");
						selectedGroup->nodes.push_back(&game_->map->nodes[0]);
					}
					
				}
			}
		}
	}
}


void RoutePlaner::draw()
{
	for (int i=0; i < buttonCount; ++i) {
		Button *button = buttons[i];
		Texture &texture = button->vehicle->tile;
		texture.set();
	
		if (i == lastClickedButtonIndex) {
			g_Renderer()->drawRectRHW(button->rect.left - 10, button->rect.top - 10, buttonSize + 20, buttonSize + 20);
		} else {
			g_Renderer()->drawRectRHW(button->rect.left, button->rect.top, buttonSize, buttonSize);
		}
	}

	//! start
	startGame.set();
	g_Renderer()->drawRectRHW(800-146, 600 - 32, 146, 32);

	//! map
	mapWidth = game_->map->width * mapScale;
	mapHeight = game_->map->height * mapScale;
	game_->map->map.set();
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	getDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	mapX = 800 - mapWidth;
	mapY = 600 - 32 - mapHeight;
	g_Renderer()->drawRectRHW(mapX, mapY, mapWidth, mapHeight);
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_Direct3D()->getDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	if (selectedGroup && selectedGroup->nodes.size() > 0) {
		D3DXVECTOR2 lastPos = selectedGroup->nodes[0]->position;
		D3DXVECTOR2 nextPos;
		D3DXVec2Add(&nextPos, &lastPos, &D3DXVECTOR2(1, 1));
		for (int i=1; i < selectedGroup->nodes.size(); ++i) {
			nextPos = selectedGroup->nodes[i]->position;
			g_Renderer()->drawLine(mapX + lastPos.x * mapScale, mapY + lastPos.y * mapScale, mapX + nextPos.x * mapScale, mapY + nextPos.y * mapScale, 5);
		}
		g_Renderer()->drawLine(mapX + lastPos.x * mapScale, mapY + lastPos.y * mapScale, mapX + nextPos.x * mapScale, mapY + nextPos.y * mapScale, 5);
	}
}