#pragma once
#include "Common.h"
#include "Map.h"
#include "Mesh.h"

#define DEG2RAD 0.0174532925

namespace EGameState
{
	enum TYPE
	{
		Intro,
		Running,
		LevelFinished,
		GameFinished
	};
};

struct SLine
{
	float x1,y1,x2,y2;
};


extern Sound* g_fireSound;
extern Sound* g_wallSound;


class Game
{
public:

	Game(void);
	~Game(void);

	void startGame();
	void changeState(EGameState::TYPE state);
	void update();
	void draw();

	//! loads level
	void loadLevel();
protected:
	static const int maxLevels_ = 6;

	EGameState::TYPE state_;
	Texture kryzys_;
	Texture crysis_;
	Texture gameScreen_;
	Texture zero_;
	Texture one_;
	Texture selection_;
	Font* introFont_;
	Map* map;
	bool leftMouseClick, leftMouseDown;

	Sound* explosionSound;
	Sound* pickSound;
	Sound* background;
	bool backgroundMusicStarted;

	Sound* typingSound;

public:
	Texture street, street_corner, street_cross3, street_cross;
	Mesh tank1;
};
