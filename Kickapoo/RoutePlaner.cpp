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
			} else {
			//! route
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

	startGame.set();
	g_Renderer()->drawRectRHW(800-146, 600 - 32, 146, 32);
}