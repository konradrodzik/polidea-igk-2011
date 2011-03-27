#include "Common.h"
#include "Game.h"

Sound* g_fireSound = NULL;
Sound* g_wallSound = NULL;
static float _introTime = 0.0f;
static float _fake;
static float _selectionAlpha = 0.0f;
static float _splashZeroElementY = 0.0f;
static float _splashOneElementY = 600.0f;
string _introText = "W roku 2010 na konferencji IGK wyciek³ kod pluginu do firefoxa\ndaj¹cy mo¿liwoœæ podró¿owania w czasie.\nNic od tamtego momentu nie by³o ju¿ takie samo.\n... a mówili, ¿e na wojnie nie ma God Mode.";
RECT _introRect;
RECT _screenMiddleRect;

const float MaxRelativeTime = 10.0f;



Game::Game(void)
	:  state_(EGameState::Intro)
,  kryzys_("kryzys_logo.jpg")
,  crysis_("crysis.jpg")
,  selection_("gfx/wave.png")
,  gameScreen_("gfx/splash_main.png")
,  zero_("gfx/splash_0.png")
,  one_("gfx/splash_1.png")
, map(NULL)
,routePlaner(NULL),
street("gfx/road.png"),
street_corner("gfx/road2.png"),
street_cross("gfx/road3.png"),
street_cross3("gfx/road4.png"),
tank1("models/tank1.x"),
grass("gfx/grass.png"),
ap_b("models/ap_b3.x"),
ap_c("models/ap_c.x"),
ap_d("models/ap_d.x"),
vip("models/car.x"),
tower("models/tower.x")
{
	g_Game = this;
	
	loadLevel();
	changeState(EGameState::RoutePlaning);

	explosionSound = g_Audio()->loadSound("sfx/explosion.wav");
	g_fireSound = g_Audio()->loadSound("sfx/fire.wav");
	pickSound = g_Audio()->loadSound("sfx/pick.wav");

	g_wallSound = g_Audio()->loadSound("sfx/wall.wav");

	background =  g_Audio()->loadSound("sfx/background.mp3", true);
	backgroundMusicStarted = false;

	typingSound = g_Audio()->loadSound("sfx/typing.wav");

		//! Create intro
	//! Fade In [0.0f - 1.0f]
	//! Wave it and FadeOut [1.0f - 2.0f]
	//! Fade In Game Screen [2.0f - 3.0f]
	//! Type text [3.0f - 3.0f + textLength * 0.1f]
	if(state_ == EGameState::Intro) {
		float totalTime = 3 + _introText.size() * 0.1f + 3;
		AnimationSequenceScalar* introTimeLine = new AnimationSequenceScalar(_introTime, 0.0f, totalTime, totalTime);
		AnimationSequenceActivator* startGame = new AnimationSequenceActivator( MakeDelegate(this, &Game::startGame) );
		introTimeLine->setNext(startGame);
		AnimationSequence::add(introTimeLine);
	}

	//! intro font
	RECT tmp={80, 530, g_Window()->getWidth(), g_Window()->getHeight()};
	_introRect = tmp;
	introFont_ = new Font();
	introFont_->create("Comic Sans MS", 40, 0, false, &_introRect);
	introFont_->setTextColor(D3DCOLOR_RGBA(127, 100, 0, 255));


	RECT screenMiddle ={g_Window()->getWidth() * 0.25f, g_Window()->getHeight()*0.5f, g_Window()->getWidth() * 0.25f + g_Window()->getWidth(),  g_Window()->getHeight() * 0.25f + g_Window()->getHeight()};
	_screenMiddleRect = screenMiddle;

}

void Game::loadLevel()
{
	delete map;
	map = NULL;
	map = Map::load("map0.bmp");
	// TODO:
	map->setupGroups();

	delete routePlaner;
	routePlaner = new RoutePlaner();
}

Game::~Game(void)
{
	delete introFont_;
}

void Game::changeState(EGameState::TYPE state)
{
	state_ = state;
	if(state_ == EGameState::Running)
	{
		map->setupGroups();
	}
}

void Game::startGame()
{
	loadLevel();
	changeState(EGameState::Running);

	
}

void Game::update()
{
	float dt = g_Timer()->getFrameTime();

	if(state_ == EGameState::Intro)
	{
	}
	else if(state_ == EGameState::RoutePlaning)
	{
		if (routePlaner)
			routePlaner->update();
	}
	else if(state_ == EGameState::Running)
	{
		if(map)
			map->update();
		for(int i = 0; i < maxLevels_; ++i) {
			if(GetKeyState('1' + i) & 0x80) {
				loadLevel();
				return;
			}
		}

		if(g_Input()->buttonClicked(0)) 
		{
			D3DXVECTOR3 p = map->cameraPosition;
			p.y = 10;

			/*Fire* fire = new Fire( p);
			g_ParticleSystem()->spawn(fire);*/

			Bullet* bullet = new Bullet(BulletType::BULLET_Rocket);
			bullet->position.x = p.x;
			bullet->position.y = p.y;
			bullet->velocity = D3DXVECTOR2(RandomFloat(0, 1), RandomFloat(0, 1));

			map->bullets.push_back(bullet);
		}
		else if(g_Input()->buttonClicked(1)) 
		{
			D3DXVECTOR3 p = map->cameraPosition;
			p.y = -100;

			Nova* nova = new Nova(p, 0.5f, 50);
			g_ParticleSystem()->spawn(nova);
		}
	}
}

void Game::draw()
{
	if(state_ == EGameState::Intro)
	{
		if(_introTime < 1.0f)
		{
			getDevice()->SetTexture(0, crysis_.getTexture());
			g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)(_introTime * 255.0f),255,255,255));
		} else
			//! Wave it and FadeOut [1.0f - 2.0f]
			if(_introTime < 2.0f)
			{
				getDevice()->SetTexture(0, crysis_.getTexture());
				g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((2.0f - _introTime) * 255.0f),255,255,255));
				getDevice()->SetTexture(0, kryzys_.getTexture());
				g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((_introTime - 1.0f ) * 255.0f),255,255,255));
			} else
				//! Fade In Game Screen [2.0f - 3.0f]
				if(_introTime < 3.0f)
				{
					getDevice()->SetTexture(0, gameScreen_.getTexture());
					g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB((int)((_introTime - 2.0f) * 255.0f),255,255,255));
					getDevice()->SetTexture(0, zero_.getTexture());
					g_Renderer()->drawRect(0, _splashZeroElementY, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));

				} else
				//! Type text [3.0f - 3.0f + textLength * 0.1f]
				//! and change 2010 into 2110
				{
					static int typeChar = 0;
					getDevice()->SetTexture(0, gameScreen_.getTexture());
					g_Renderer()->drawRect(0, 0, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));

					string typedText;
					int currentLength = (int)((_introTime - 3.0f) * 10.0f);
					typedText.assign(_introText.c_str(), currentLength);
					introFont_->write(typedText.c_str());
					float totalTime = 3 + _introText.size() * 0.1f;

					if(currentLength > typeChar && _introTime <= totalTime)
					{
						typeChar = currentLength;

						if(typedText[typedText.length() - 1] != ' ')
							g_Audio()->play(typingSound);
					}

					if(_splashOneElementY > 0.0f) {
						float speed =  600.0f / 1.5f;
						_splashZeroElementY -= speed * g_Timer()->getFrameTime();
						_splashOneElementY -= speed * g_Timer()->getFrameTime();
					};
					getDevice()->SetTexture(0, zero_.getTexture());
					g_Renderer()->drawRect(0, _splashZeroElementY, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
					getDevice()->SetTexture(0, one_.getTexture());
					g_Renderer()->drawRect(0, _splashOneElementY, g_Window()->getWidth(), g_Window()->getHeight(), D3DCOLOR_ARGB(255,255,255,255));
				}

	} 
	else if(state_ == EGameState::RoutePlaning)
	{
		if(routePlaner)
			routePlaner->draw();
	} else 
	{
		if(map)
			map->draw();
	}
	
}
