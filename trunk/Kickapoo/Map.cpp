#include "Common.h"

#define GROUP_BUTTON_SIZE 50

Bullet::Bullet(BulletType type) : type(type), position(0,0), velocity(0,0), owner(NULL), smoke(NULL)
{
	splashRange = 1;
	texture = "gfx/missile.png";
	if(type == BulletType::BULLET_Rocket)
	{
		smoke = new Smoke();
		g_ParticleSystem()->spawn(smoke);
	}
}

void Bullet::update()
{
	D3DXVec2Normalize(&velocity, &velocity);
	position = position + velocity * g_Timer()->getFrameTime();
	if(type == BulletType::BULLET_Rocket)
	{
		smoke->pos = D3DXVECTOR3(position.x - 0.2, 0, position.y - 0.2);
	}
}

void Bullet::draw()
{
	texture.set();
	g_Renderer()->drawRotatedRectZ(position.x - 0.2, position.y - 0.2, 0.2f, 0.2f, velocity);
}

Bullet* Vehicle::fire(D3DXVECTOR2 dir)
{
	if(fireTime > 1.0f)
	{
		fireTime = 0;
		Bullet* bullet = new Bullet(bulletType);
		bullet->position = center();
		bullet->velocity = dir;
		bullet->owner = this;
		bullet->group = group;
	
		return bullet;
	}
	return NULL;
}

void Vehicle::update()
{
	if(!group->started)
		return;

	fireTime += g_Timer()->getFrameTime();
	time += g_Timer()->getFrameTime();

	if(time > startTime)
	{
		if(driving)
		{
			if(currentNode + 1 < group->nodes.size())
			{
				nextPos = group->nodes[currentNode + 1]->position;
				dir = nextPos - group->nodes[currentNode]->position;
				D3DXVec2Normalize(&dir, &dir);
			}
			position = position + dir * velocity * g_Timer()->getFrameTime();

			if(D3DXVec2Length(&(nextPos - position)) < 0.1) 
			{
				currentNode++;
				if(currentNode > group->nodes.size()) {
					dir = D3DXVECTOR2(0, 0);
					driving = false;
				}
			}
		}
	}
}

Map::Map() : cameraPosition(20,6,15)
{
	width = height = 0;
	nodeCount = vehicleCount = 0;
	startNode = NULL;
	camera_mode = false;
}

Map::~Map()
{
}

void Map::setupGroups()
{
	groupCount = 4;

#if 1
	if(vehicleCount == 0)
	{
		for(int i = 0; i < 16; ++i)
		{
			Vehicle& v = vehicles[vehicleCount++];
			v.tile.load("police.png");
			v.group = &groups[i % groupCount];
		}
	}
	
	for(int i = 0; i < vehicleCount; ++i)
	{
		if(startNode)
			vehicles[i].position = startNode->position;
		else
			vehicles[i].position = nodes[0].position;
		vehicles[i].dir = D3DXVECTOR2(0,1);
	}
#else
	int groupIndex = 0;
	for (int i=0; i < 16; ++i) {
		if (i > 3 && i % 4 == 0) groupIndex++;
		vehicles[i].group = &groups[groupIndex];
		vehicles[i].tile = Texture( i % 2 == 0 ? "taxi.png" : "merc.png" );
		int nodeIndex = i % nodeCount;
		if(startNode)
			vehicles[i].position = startNode->position;
		else
			vehicles[i].position = nodes[0].position;
	}
	vehicleCount = 16;
#endif

	int vip = rand() % vehicleCount;
	vehicles[vip].type = VEHICLE_Vip;
	vehicles[vip].tile.load("merc.png");
	/*
	for(int i = 0; i < nodeCount; ++i)
	{
		groups[0].nodes.push_back(&nodes[i]);
	}
	*/
	D3DXVECTOR2 startPos = D3DXVECTOR2((g_Window()->getWidth() - GROUP_BUTTON_SIZE * groupCount) / 2, 
		g_Window()->getHeight() - GROUP_BUTTON_SIZE);
	float groupRay = GROUP_BUTTON_SIZE;
	for(int i = 0; i < groupCount; ++i)
	{
		groups[i].pos = startPos + D3DXVECTOR2((groupRay + 10) * i, 0);
	}
}

static void trimr(char * buffer) {
	unsigned length = strlen(buffer);
	while(buffer[0] && (buffer[length-1] == '\n' || buffer[length-1] == '\r'))
		buffer[length-1] = 0;
}

const char* basename(const char* p) {
	if(strlen(p) >= 3)
		return &p[strlen(p)-3];
	return "";
}

Map* Map::load( const std::string& name )
{
	FILE* file = fopen(name.c_str(), "rb");
	if(!file)
		return NULL;

	Map* map = new Map;

	if(!strcmpi(basename(name.c_str()), "bmp")) {
		BITMAPFILEHEADER bmfh;
		fread(&bmfh,sizeof(BITMAPFILEHEADER),1,file);
		BITMAPINFOHEADER bmih;
		fread(&bmih,sizeof(BITMAPINFOHEADER),1,file);

		fseek(file, bmfh.bfOffBits, SEEK_SET);

		map->width = bmih.biWidth;
		map->height = bmih.biHeight;

		assert(bmih.biBitCount == 24);
		assert(bmih.biCompression == 0);

		unsigned pad = (LONG)((float)map->width*(float)bmih.biBitCount/8.0);
		unsigned byteWidth = pad;

		//add any extra space to bring each line to a DWORD boundary
		while(pad%4!=0) {
			 pad++;
		}

		for(unsigned i = map->height; i-- > 0; ) {
			byte bytes[3];
			for(unsigned j = 0; j < map->width; ++j) {
				int h = i;//map->height - i - 1;
				int index = (map->height - i - 1) * map->width + j;
				fread(bytes, 3, 1, file);
				std::swap(bytes[0], bytes[2]);
				if(bytes[0]>80 && bytes[1]>80 && bytes[2]>80)
					continue;

				if(bytes[0] > 80 && bytes[1] <= 80 && bytes[2] <= 80) {
					map->tiles[j][h].type = TILE_Street;
				}
				else if(bytes[0] > 80 && bytes[1] > 80 && bytes[2] <= 80) {
					map->tiles[j][h].type = TILE_Street;
					map->tiles[j][h].isStart = true;
				}
				else if(bytes[0] <= 80 && bytes[1] <= 80 && bytes[2] > 80) {
					map->tiles[j][h].type = TILE_Water;
				}
				else if(bytes[0] <= 80 && bytes[1] <= 80 && bytes[2] <= 80) {
					map->tiles[j][h].type = TILE_Block;
					map->tiles[j][h].random = 1;// + 0.5 * (float)rand() / RAND_MAX;
				}
				else if(bytes[0] >80 && bytes[1] <= 80 && bytes[2]> 80) {
					map->tiles[j][h].type = TILE_Street;
					map->tiles[j][h].random = 1;
					map->tiles[j][h].isEnd = true;
				}
				else if(bytes[0] <= 80 && bytes[1] > 80 && bytes[2] <= 80) {
						map->tiles[j][h].texture = &g_Game->grass;
					//if(i & 2 || j & 2)
					//	continue;

					Tower* tower = new Tower;
					map->towers.push_back(tower);
					tower->type = TOWER_Fast;
					tower->pos = D3DXVECTOR2(j,h);
					tower->hp = 100;
					tower->mesh = &g_Game->tower;
				}
			}
			fread(bytes, pad-byteWidth, 1, file);
		}
	}

	fclose(file);

	map->map.load(name, -1);

	map->finalize();
	return map;
}

void Map::finalize()
{	
	nodeCount = 0;
	vehicleCount = 0;
	groupCount = 0;

	for(int i = 1; i < height-1; ++i)
	{
		for(int j = 1; j < width-1; ++j)
		{
			Tile& cur = tiles[j][i];
			const Tile& left = tiles[j-1][i];
			const Tile& right = tiles[j+1][i];
			const Tile& up = tiles[j][i-1];
			const Tile& down = tiles[j][i+1];

			if(cur.type == TILE_Street)
			{
				if(left.type == right.type && left.type == TILE_Street && up.type == down.type && down.type == TILE_Street)
				{
					cur.texture = &g_Game->street_cross;
				}
				else if(right.type == left.type && up.type == right.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_cross3;
					cur.offset = 2;
				}
				else if(right.type == left.type && down.type == right.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_cross3;
					cur.offset = 0;
				}
				else if(down.type == up.type && up.type == right.type && down.type == TILE_Street)
				{
					cur.texture = &g_Game->street_cross3;
					cur.offset = 3;
				}
				else if(down.type == up.type && up.type == left.type && down.type == TILE_Street)
				{
					cur.texture = &g_Game->street_cross3;
					cur.offset = 1;
				}
				else if(left.type == right.type && left.type == TILE_Street)
				{
					cur.texture = &g_Game->street;
				}
				else if(up.type == down.type && down.type == TILE_Street)
				{
					cur.texture = &g_Game->street;
					cur.offset = 2;
				}
				else if(left.type == up.type && left.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 1;
				}
				else if(left.type == down.type && left.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 0;
				}
				else if(right.type == down.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 3;
				}
				else if(right.type == up.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 2;
				}
				else if(up.type == cur.type || down.type == cur.type)
				{
					cur.texture = &g_Game->street;
					cur.offset = 2;
				}
				else
				{
					cur.texture = &g_Game->street;
				}


				if(cur.texture != &g_Game->street || cur.isStart || cur.isEnd)
				{
					// add node
					cur.node = &nodes[nodeCount++];
					cur.node->position = D3DXVECTOR2(j,i);
					cur.node->otherNodes.clear();
				}

				if(cur.isStart)
				{
					startNode = cur.node;
				}

				if(cur.isEnd)
				{
					cur.node->hangar = true;
					cur.texture = &g_Game->grass;
					if(i & 2 || j & 2)
						continue;
					cur.mesh = &g_Game->hangar;
				}
			}
			else if(cur.type == TILE_BUNKIER)
			{				
				cur.texture = &g_Game->grass;
					if(i & 2 || j & 2)
						continue;
					cur.mesh = &g_Game->hangar;
			}
			else if(cur.type == TILE_Block)
			{
						cur.texture = &g_Game->grass;
					//if(i & 2 || j & 2)
					//	continue;

					switch(rand() % 3)
					{
					case 0:
				cur.mesh = &g_Game->ap_c;
				break;
				
					case 2:
				cur.mesh = &g_Game->ap_c;
				break;
				
					case 1:
				cur.mesh = &g_Game->ap_d;
				break;
					}
			}
			else
			{
				cur.texture = &g_Game->grass;
			}
		}
	}

	for(int w = 1; w < width-1; ++w)
	{
		Node* lastNode = NULL;

		for(int h = 1; h < height-1; ++h)
		{
			Tile& cur = tiles[w][h];
			if(cur.type != TILE_Street)
			{
				lastNode = NULL;
				continue;
			}

			Node* node = cur.node;
			if(!node)
				continue;
			
			if(cur.texture == &g_Game->street && !cur.isStart)
			{
				if(!lastNode)
					lastNode = node;
				continue;
			}

			if(lastNode)
			{
				lastNode->otherNodes.push_back(node);
				node->otherNodes.push_back(lastNode);
			}
			lastNode = node;
		}
	}
	
	for(int h = 1; h < height-1; ++h)
	{
		Node* lastNode = NULL;
		
		for(int w = 1; w < width-1; ++w)
		{
			Tile& cur = tiles[w][h];
			if(cur.type != TILE_Street)
			{
				lastNode = NULL;
				continue;
			}

			Node* node = cur.node;
			if(!node)
				continue;

			if(cur.texture == &g_Game->street)
			{
				if(!lastNode)
					lastNode = node;
				continue;
			}

			if(lastNode)
			{
				lastNode->otherNodes.push_back(node);
				node->otherNodes.push_back(lastNode);
			}
			lastNode = node;
		}
	}

	for(int i = 0; i < nodeCount; ++i)
	{
		Node* node = &nodes[i];
		printf("Node %.fx%.f:\n", node->position.x, node->position.y);
		for(int j = 0; j < node->otherNodes.size(); ++j)
		{
			Node* other = node->otherNodes[j];
			printf("  -  %.fx%.f:\n", other->position.x, other->position.y);
		}
	}
}

void Map::startGroup(int i)
{
	g_Audio()->play(g_Game->pickSound);
	groups[i].started = true;
	for(int j = 0; j < 4; ++j)
	{
		Vehicle* vehicle = g_Game->routePlaner->buttons[i * 4 + j]->vehicle;
		if(vehicle->type != VEHICLE_NONE)
		{
			// start vehicle every 2 seconds
			vehicle->startTime = j * 2;
		}
	}
}

void Map::update()
{
	int dx, dy;
	int scroll;
	g_Input()->getMovement(dx, dy);
	g_Input()->getScroll(scroll);
	float sensitivity = 3;
	cameraPosition.x += dx * sensitivity * g_Timer()->getFrameTime(); 
	cameraPosition.z += dy * sensitivity * g_Timer()->getFrameTime();
	cameraPosition.x = max(0, min(width, cameraPosition.x));
	cameraPosition.z = max(0, min(height, cameraPosition.z));

	if(g_Input()->keyPressed(VK_SPACE))
		camera_mode = !camera_mode;

	//if(scroll > 0)
	//	cameraPosition.y = max(cameraPosition.y - 3, 3);
	//else if(scroll < 0)
	//	cameraPosition.y = min(cameraPosition.y + 3, 100);

	// start groups
	if((GetKeyState('Q') & 0x80) && !groups[0].started)
	{
		startGroup(0);
	}
	else if((GetKeyState('W') & 0x80) && !groups[1].started)
	{
		startGroup(1);
	}
	else if((GetKeyState('E') & 0x80) && !groups[2].started)
	{
		startGroup(2);
	}
	else if((GetKeyState('R') & 0x80) && !groups[3].started)
	{
		startGroup(3);
	}

	for(int i = 0; i < bullets.size(); )
	{
		bullets[i]->update();
		// TODO: collisions
		int x = bullets[i]->position.x, y = bullets[i]->position.y;
		if(x < 0 || y < 0 || x > MaxMapSize || y > MaxMapSize || tiles[x][y].type == TileType::TILE_Block)
		{
			g_ParticleSystem()->spawn(new Nova(D3DXVECTOR3(bullets[i]->center().x, 
							0, bullets[i]->center().y), 0.3f, 2));
			if(bullets[i]->smoke != NULL) {
				bullets[i]->smoke->enabled = false;
			}
			bullets.erase(bullets.begin() + i);
			goto END;
		}

		for(int j = 0; j < vehicleCount; ++j)
		{
			if(vehicles[j].hp > 0 && bullets[i]->owner != &vehicles[j] && bullets[i]->group != vehicles[j].group)
			{
				float dist = D3DXVec2Length(&(bullets[i]->center() - vehicles[j].center()));
				if(dist < 0.2f)
				{
					vehicles[j].hp -= bullets[i]->demage();
					// TODO: particle
					//if(bullets[i]->type == BulletType::BULLET_Rocket)
					{
						g_ParticleSystem()->spawn(new Nova(D3DXVECTOR3(bullets[i]->center().x, 
							0, bullets[i]->center().y), 0.3f, 2));
					}
					/*else if(bullets[i]->type == BulletType::BULLET_Shot) 
					{
						g_ParticleSystem()->spawn(new Fire(D3DXVECTOR3(bullets[i]->center().x, 
							0, bullets[i]->center().y)));
					}*/
					if(bullets[i]->smoke != NULL)
					{
						bullets[i]->smoke->enabled = false;
					}
					bullets.erase(bullets.begin() + i);
					goto END;
				}
			}
		}

		for(int j = 0; j < towers.size(); ++j)
		{
			if(towers[j]->hp > 0 && bullets[i]->owner != towers[j])
			{
				float dist = D3DXVec2Length(&(bullets[i]->center() - towers[j]->center()));
				if(dist < 0.5f)
				{
					towers[j]->hp -= bullets[i]->demage();
					// TODO: particle
					//if(bullets[i]->type == BulletType::BULLET_Rocket)
					{
						g_ParticleSystem()->spawn(new Nova(D3DXVECTOR3(bullets[i]->center().x, 
							0, bullets[i]->center().y), 0.3f, 2));
					}
					/*else if(bullets[i]->type == BulletType::BULLET_Shot) 
					{
						g_ParticleSystem()->spawn(new Fire(D3DXVECTOR3(bullets[i]->center().x, 
							0, bullets[i]->center().y)));
					}*/
					if(bullets[i]->smoke != NULL)
					{
						bullets[i]->smoke->enabled = false;
					}
					bullets.erase(bullets.begin() + i);
					goto END;
				}
			}
		}

		++i;
END:
		;
	}

	for(int j = 0; j < vehicleCount; ++j)
	{
		if(vehicles[j].hp > 0)
		{
			vehicles[j].update();
			
			// wygrana
			if(vehicles[j].type == VehicleType::VEHICLE_Vip)
			{
				int x = vehicles[j].center().x, y = vehicles[j].center().y;
				if(x >= 0 && y >= 0 && x < MaxMapSize && y < MaxMapSize && tiles[x][y].type == TileType::TILE_BUNKIER)
				{
					g_Game->changeState(EGameState::GameFinished);
				}
			}
		}
		// przegrana
		else if(vehicles[j].type == VehicleType::VEHICLE_Vip) 
		{
			g_Game->changeState(EGameState::GameFinished);
		}
	}

	for(int i = 0; i < towers.size(); ++i)
	{
		if(towers[i]->hp <= 0)
		{
			continue;
		}

		towers[i]->update();
		for(int j = 0; j < vehicleCount; ++j)
		{
			if(vehicles[j].hp > 0 && vehicles[j].time > vehicles[j].startTime)
			{
				float dist = D3DXVec2Length(&(towers[i]->center() - vehicles[j].center()));
				if(dist < towers[i]->range)
				{
					Bullet* bullet = towers[i]->fire(vehicles[j].center() - towers[i]->center());
					if(bullet != NULL)
					{
						bullets.push_back(bullet);
					}
				}

				if(dist < vehicles[j].range)
				{
					Bullet* bullet = vehicles[j].fire(towers[i]->center() - vehicles[j].center());
					if(bullet != NULL)
					{
						bullets.push_back(bullet);
					}
				}
			}
		}
	}
}

#define COUNT_OF(x) (sizeof(x)/sizeof((x)[0]))

struct Vertex
{
	float xyz[3];
	float normal[3];
	float tex[2];
	static DWORD FVF;
};

DWORD Vertex::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2;

static Vertex RectVerts[4] = {
	{0,0,0,  0,0,1,  0,0},
	{0,0,1,  0,0,1,  0,1},
	{1,0,1,  0,0,1,  1,1},
	{1,0,0,  0,0,1,  1,0}
};

static Vertex BoxVerts[] = {
    {1, 1, 1,  1, 0, 0}, {1, 0, 1,  1, 0, 0}, {1, 0, 0,  1, 0, 0},
    {1, 1, 0,  1, 0, -0}, {1, 1, 1,  1, 0, -0}, {1, 0, 0,  1, 0, -0},
    {0, 1, 1,  -1, 0, -0}, {0, 1, 0,  -1, 0, -0}, {0, 0, 0,  -1, 0, -0},
    {0, 0, 1,  -1, 0, 0}, {0, 1, 1,  -1, 0, 0}, {0, 0, 0,  -1, 0, 0},
    {1, 1, 1,  0, 0, 1}, {0, 1, 1,  0, 0, 1}, {0, 0, 1,  0, 0, 1},
    {1, 0, 1,  0, -0, 1}, {1, 1, 1,  0, -0, 1}, {0, 0, 1,  0, -0, 1},
    {1, 1, 0,  0, -0, -1}, {1, 0, 0,  0, -0, -1}, {0, 0, 0,  0, -0, -1},
    {0, 1, 0,  0, 0, -1}, {1, 1, 0,  0, 0, -1}, {0, 0, 0,  0, 0, -1},
    {1, 1, 1,  0, 1, 0}, {1, 1, 0,  0, 1, 0}, {0, 1, 0,  0, 1, 0},
    {0, 1, 1,  -0, 1, 0}, {1, 1, 1,  -0, 1, 0}, {0, 1, 0,  -0, 1, 0},
    {1, 0, 1,  -0, -1, 0}, {0, 0, 1,  -0, -1, 0}, {0, 0, 0,  -0, -1, 0},
    {1, 0, 0,  0, -1, 0}, {1, 0, 1,  0, -1, 0}, {0, 0, 0,  0, -1, 0},
};

void drawRing(Texture& texture, D3DXVECTOR2 position, float shift)
{	
	float angle = GetTickCount() / 80.0f + shift;
	float x = 0.8f + 0.1f*sinf(angle);
	Vertex verts[COUNT_OF(RectVerts)];
	for(int k = 0; k < COUNT_OF(RectVerts); ++k)
	{
		verts[k] = RectVerts[k];
		verts[k].xyz[0] = (verts[k].xyz[0] * 2.0f - 1.0f) * x * 0.5f + 0.5f;
		verts[k].xyz[2] = (verts[k].xyz[2] * 2.0f - 1.0f) * x * 0.5f + 0.5f;
		verts[k].xyz[1] = 0.01f;
		verts[k].xyz[0] += position.x;
		verts[k].xyz[2] += position.y;
	}
				
	texture.set(0);
	getDevice()->SetFVF(Vertex::FVF);
	getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, verts, sizeof(verts[0]));
}

void Map::drawTiles()
{
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			Tile& tile = tiles[j][i];

			if(tile.mesh)
			{
				tile.mesh->draw(D3DXVECTOR3(j + 0.5f, 0, i + 0.5f));
				tile.texture = &g_Game->grass;
			}
			 
			if(!tile.texture)
				continue;
			Vertex verts[COUNT_OF(RectVerts)];
			for(int k = 0; k < COUNT_OF(RectVerts); ++k)
			{
				verts[k] = RectVerts[k];
				verts[k].xyz[0] += j;
				verts[k].xyz[2] += i;
				verts[k].tex[0] = RectVerts[(k+tile.offset)%COUNT_OF(RectVerts)].tex[0];
				verts[k].tex[1] = RectVerts[(k+tile.offset)%COUNT_OF(RectVerts)].tex[1];
			}
				
			tile.texture->set(0);
			getDevice()->SetFVF(Vertex::FVF);
			getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, verts, sizeof(verts[0]));
		}
	}
}

void Map::drawNodes(D3DXMATRIX* trans)
{
	LPD3DXLINE line;
	HRESULT hr = D3DXCreateLine(getDevice(), &line);
	if(FAILED(hr))
		return;
	line->SetWidth(5.0f);
	line->SetAntialias(TRUE);
	line->SetGLLines(TRUE);

	line->Begin();

#if 1
	D3DCOLOR colors[MaxMapGroups] =
	{	
		D3DCOLOR_ARGB(100, 255, 150, 120),
		D3DCOLOR_ARGB(100, 120, 255, 120),
		D3DCOLOR_ARGB(100, 120, 120, 255),
		D3DCOLOR_ARGB(100, 255, 255, 120),
	};

	for(int i = 0; i < MaxMapGroups; ++i)
	{
		Group* group = &groups[i];
		if(group->nodes.empty())
			continue;
		Node* lastNode = group->nodes[0];
		for(int j = 1; j < group->nodes.size(); ++j)
		{
			Node* node = group->nodes[j];

			D3DXVECTOR3 v[2];

			v[0].x = 0.5f+lastNode->position.x;
			v[0].z = 0.5f+lastNode->position.y;
			v[0].y = 0.02f;
			v[1].x = 0.5f+node->position.x;
			v[1].z = 0.5f+node->position.y;
			v[1].y = 0.02f;
			
			line->DrawTransform(v, 2, trans, colors[i]);

			lastNode = node;
		}
	}

#else
	for(int i = 0; i < nodeCount; ++i)
	{
		Node* a = &nodes[i];
		for(int j = 0; j < a->otherNodes.size(); ++j)
		{
			Node *b = a->otherNodes[j];
			if(b <= a)
				continue;

			D3DXVECTOR3 v[2];

			v[0].x = 0.5f+a->position.x;
			v[0].z = 0.5f+a->position.y;
			v[0].y = 0.02f;
			v[1].x = 0.5f+b->position.x;
			v[1].z = 0.5f+b->position.y;
			v[1].y = 0.02f;

			D3DCOLOR color = D3DCOLOR_ARGB(255, (j%3) == 0 ? 100 : 200, 
				(j%3) == 1 ? 100 : 200, 
				(j%3) == 2 ? 100 : 200);
			
			line->DrawTransform(v, 2, trans, color);
		}
	}
#endif

	line->End();

	line->Release();
}

void Map::draw()
{
	getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128,128,128), 1.0f, 0);

	// setup projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovRH(&proj, 60.0f / 180 * 3.14f, 4.0f/3.0f, 0.2f, 1000);
	getDevice()->SetTransform(D3DTS_PROJECTION, &proj);

	// setup view matrix
	D3DXMATRIX view;
	D3DXVECTOR3 eye, at, up(0,1,0);
	eye = cameraPosition;
	at = cameraPosition;

	at.x += 0;
	at.z -= 3;
	at.y = 0;

	if(camera_mode)
	{
		for(int i = 0; i < vehicleCount; ++i)
		{
			if(vehicles[i].type == VEHICLE_Vip)
			{
				D3DXVECTOR2 dir;
				D3DXVec2Subtract(&dir, &nodes[vehicles[i].currentNode].position, &vehicles[i].position);
				D3DXVec2Normalize(&dir, &dir);
				eye = D3DXVECTOR3(vehicles[i].position.x, 1, vehicles[i].position.y);
				D3DXVECTOR3 dir2(dir.x, 0, dir.y);
				at.x = eye.x + dir2.x;
				at.y = eye.y + dir2.y;
				at.z = eye.z + dir2.z;
			}
		}
	}
	D3DXMatrixLookAtRH(&view, &eye, &at, &up);
	getDevice()->SetTransform(D3DTS_VIEW, &view);

	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &view, &proj);

	// setup drawing options
	getDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	getDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// setup material
	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	material.Ambient.r  = 1.0f;
	material.Ambient.g  = 1.0f;
	material.Ambient.b  = 1.0f;
	material.Ambient.a  = 1.0f;
	material.Diffuse.r  = 1.0f;
	material.Diffuse.g  = 1.0f;
	material.Diffuse.b  = 1.0f;
	material.Diffuse.a  = 1.0f;
	getDevice()->SetMaterial(&material);

	// setup lighting
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_POINT;
	light.Position = at;
	light.Ambient.r  = 0.2f;
	light.Ambient.g  = 0.2f;
	light.Ambient.b  = 0.2f;
	light.Ambient.a  = 0.2f;
	light.Diffuse.r  = 0.8f;
	light.Diffuse.g  = 0.8f;
	light.Diffuse.b  = 0.8f;
	light.Diffuse.a  = 0.8f;
	light.Specular.r  = 0.4f;
	light.Specular.g  = 0.4f;
	light.Specular.b  = 0.4f;
	light.Specular.a  = 1.0f;
	light.Attenuation1 = 0.2f;
	light.Range      = 20.0f;
	light.Phi = 40/180*3.14;
	light.Theta = 80/180*3.14;
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(0.0f,-1.0,0.3);
	//D3DXVec3Subtract((D3DXVECTOR3*)&light.Direction, &eye, &at);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, (D3DXVECTOR3*)&light.Direction);
	getDevice()->SetLight(0, &light);
	getDevice()->LightEnable(0, TRUE);
	getDevice()->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(25,25,25));
	int lightIndex = 1;
	// draw map
	drawTiles();

	// draw nodes
	drawNodes(&matrix);

	for(int i = 0; i < towers.size(); ++i)
	{
 		Tower* t = towers[i];
		if(t->mesh && t->hp > 0)
		{
			D3DXVECTOR2 vv(t->pos.x, t->pos.y);
			t->mesh->draw(D3DXVECTOR3(t->pos.x + 0.5f, 0, t->pos.y + 0.5f));
			drawRing(g_Game->ringa, vv, 10 + i);
		}
	}

	for(int i = 0; i < vehicleCount; ++i)
	{
		Vehicle& v = vehicles[i];
		Mesh* mesh = NULL;
		switch(v.type)
		{
		case VEHICLE_NONE:
			break;

		case VEHICLE_Fast:
		case VEHICLE_Slow:
			mesh = &g_Game->police;
			break;

		case VEHICLE_Vip:
			mesh = &g_Game->vip;
			break;
		}
		if(!mesh)
			continue;

		D3DXVECTOR2 dir;
		D3DXVec2Subtract(&dir, &nodes[v.currentNode].position, &v.position);
		D3DXVec2Normalize(&dir, &dir);
		D3DXVECTOR2 vv(v.position.x + 0.5f, v.position.y + 0.5f);
		mesh->draw(D3DXVECTOR3(v.position.x + 0.5f, 0.02f, v.position.y + 0.5f), D3DXVECTOR3(dir.x, 0, dir.y));
		if(v.type == VEHICLE_Vip)
			drawRing(g_Game->ringc, v.position, 3 + i);
		//else
		//	drawRing(g_Game->ringb, v.position, 5 + i);

		if(lightIndex < 8 && false)
		{
			memset(&light.Ambient, 0, sizeof(light.Ambient));
			light.Position = D3DXVECTOR3(v.position.x + 0.5f, 0, v.position.y + 0.5f);
			light.Range = 3.0f;
			light.Attenuation1 = 0.5f;
			getDevice()->SetLight(lightIndex, &light);
			getDevice()->LightEnable(lightIndex, TRUE);
			++lightIndex;
		}
	}

	while(lightIndex<8)
	{
		getDevice()->LightEnable(lightIndex++, FALSE);
	}
	
	// setup drawing options
	getDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// disable z
	getDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	for(int i = 0; i < bullets.size(); ++i)
	{
		bullets[i]->draw();
	}

	g_ParticleSystem()->renderParticles();

	getDevice()->SetTexture(0, NULL);
	// draw groups buttons
	for(int i = 0; i < groupCount; ++i)
	{
		g_Renderer()->drawRectRHW(groups[i].pos.x - GROUP_BUTTON_SIZE / 2,
			groups[i].pos.y - GROUP_BUTTON_SIZE / 2, GROUP_BUTTON_SIZE, GROUP_BUTTON_SIZE, 
			groups[i].started ? D3DXCOLOR(0, 1, 0, 1) : D3DXCOLOR(1, 0, 0, 1));
	}
}
