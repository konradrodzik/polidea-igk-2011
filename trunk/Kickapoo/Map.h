#pragma once
#include "Common.h"

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

	Bullet() : type(BULLET_Shot), position(0,0), velocity(0,0)
	{
	}
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

	Vehicle() : type(VEHICLE_Fast), bulletType(BULLET_Shot), model(NULL), group(NULL)
	{
	}
};

struct Group
{
	vector<Node*> nodes;
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

	Tile() : type(TILE_Ignore), random(0), texture(0), node(NULL)
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

	D3DXVECTOR3 cameraPosition;

public:
	Map();

	~Map();

	static Map* load(const std::string& name);

	void update();
	void finalize();

	void draw();
	void drawTiles();
	void drawNodes(D3DXMATRIX* trans);

public:

};