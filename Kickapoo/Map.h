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

	Node() : position(0,0)
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

	Bullet(BulletType type);

	void update();
	void draw();
};

enum VehicleType
{
	VEHICLE_Fast,
	VEHICLE_Slow
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

	Vehicle() : type(VEHICLE_Fast), bulletType(BULLET_Shot), model(NULL), group(NULL), currentNode(0), velocity(10)
	{
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

	Tower() : type(TOWER_Fast)
	{
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
	TILE_Block
};

struct Tile
{
	TileType type;
	float random;
	Texture* texture;
	int offset;
	Node* node;
	Mesh* mesh;

	Tile() : type(TILE_Ignore), random(0), texture(0), node(NULL), mesh(NULL)
	{
	}
};

class Map
{
public:
	Tile tiles[MaxMapSize][MaxMapSize]; // [width][height]
	int width, height;

	Node nodes[MaxMapNodes];
	int nodeCount;
	
	Vehicle vehicles[MaxMapVehicles];
	int vehicleCount;

	Group groups[MaxMapGroups];
	int groupCount;

	std::vector<Bullet*> bullets;

	D3DXVECTOR3 cameraPosition;
	Texture map;

public:
	Map();

	~Map();

	static Map* load(const std::string& name);

	void setupGroups();

	void update();
	void finalize();

	void draw();
	void drawTiles();
	void drawNodes(D3DXMATRIX* trans);

public:

};