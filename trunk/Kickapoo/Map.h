#pragma once
#include "Common.h"
#include "Mesh.h"

class Smoke;

const int MaxMapSize = 800;
const int MaxMapNodes = 400;
const int MaxMapVehicles = 16;
const int MaxMapGroups = 4;

struct Node
{
	D3DXVECTOR2 position;
	vector<Node*> otherNodes;
	bool hangar;

	Node() : position(0,0), hangar(false)
	{
	}
};

enum BulletType
{
	BULLET_Shot,
	BULLET_Rocket
};

struct Bullet
{
	BulletType type;
	D3DXVECTOR2 position, velocity;
	Smoke* smoke;
	Texture texture;
	float splashRange;

	Bullet(BulletType type);

	void update();
	void draw();

	float demage()
	{
		if(type == BulletType::BULLET_Shot)
		{
			return 1;
		}
		else if(type == BulletType::BULLET_Rocket)
		{
			return 10;
		}
	}
};

enum VehicleType
{
	VEHICLE_NONE,
	VEHICLE_Fast,
	VEHICLE_Slow,
	VEHICLE_Vip
};

struct Group;

struct Vehicle
{
	VehicleType type;
	BulletType bulletType;
	int hp;
	Texture tile;
	void* model;
	D3DXVECTOR2 position;
	Group* group;
	int currentNode;
	float velocity;
	D3DXVECTOR2 dir;
	float range;
	float fireTime;
	float time;
	float startTime;
	D3DXVECTOR2 nextPos;
	bool driving;

	Vehicle() : type(VEHICLE_Fast), bulletType(BULLET_Rocket), model(NULL), group(NULL), currentNode(0), velocity(1), position(0,0),
		dir(0,0), range(5), fireTime(0), time(0), startTime(0)
	{
		hp = 300;
		driving = true;
	}

	Bullet* fire(D3DXVECTOR2 dir);
	void update();
};

struct Group
{
	vector<Node*> nodes;
	bool started;
	D3DXVECTOR2 pos;

	Group()
	{
		started = false;
	}
};

enum TowerType
{
	TOWER_Fast,
	TOWER_Slow
};

struct Tower
{
	TowerType type;
	D3DXVECTOR2 pos;
	float fireTime;
	float range;
	float hp;
	Mesh* mesh;

	Tower() : type(TOWER_Fast), mesh(NULL), range(0), fireTime(0), pos(0,0)
	{
		hp = 300;
		range = 2;
	}

	void update()
	{
		fireTime += g_Timer()->getFrameTime();
	}

	Bullet* fire(D3DXVECTOR2 dir)
	{ 
		if(type == TowerType::TOWER_Fast && fireTime > 0.5f)
		{
			fireTime = 0;
			Bullet* bullet = new Bullet(BulletType::BULLET_Shot);
			bullet->position = pos;
			bullet->velocity = dir;
			return bullet;
		}
		else if(type == TowerType::TOWER_Slow && fireTime > 2.0f)
		{
			fireTime = 0;
			Bullet* bullet = new Bullet(BulletType::BULLET_Rocket);
			bullet->position = pos;
			bullet->velocity = dir;
			return bullet;
		}
		return NULL;
	}
};

enum TileType
{
	TILE_Ignore,
	TILE_TowerFast,
	TILE_TowerSlow,
	TILE_Street,
	TILE_Water,
	TILE_Grass,
	TILE_Block,
	TILE_BUNKIER
};

struct Tile
{
	TileType type;
	float random;
	Texture* texture;
	int offset;
	Node* node;
	Mesh* mesh;
	bool isStart;
	bool isEnd;

	Tile() : type(TILE_Ignore), random(0), texture(0), node(NULL), mesh(NULL), isStart(false), isEnd(false)
	{
	}
};

class Map
{
public:
	Tile tiles[MaxMapSize][MaxMapSize]; // [width][height]
	int width, height;

	Node nodes[MaxMapNodes];
	Node* startNode;
	int nodeCount;
	
	Vehicle vehicles[MaxMapVehicles];
	int vehicleCount;

	Group groups[MaxMapGroups];
	int groupCount;

	std::vector<Tower*> towers;
	std::vector<Bullet*> bullets;

	D3DXVECTOR3 cameraPosition;
	Texture map;
	bool camera_mode;

public:
	Map();

	~Map();

	static Map* load(const std::string& name);

	void setupGroups();

	void startGroup(int i);
	void update();
	void finalize();

	void draw();
	void drawTiles();
	void drawNodes(D3DXMATRIX* trans);

public:

};