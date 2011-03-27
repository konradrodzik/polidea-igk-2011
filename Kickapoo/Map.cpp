#include "Common.h"

#define GROUP_BUTTON_SIZE 50

Bullet::Bullet(BulletType type) : type(type), position(0,0), velocity(0,0)
{
	if(type == BulletType::BULLET_Rocket)
	{
		texture = "gfx/player_selected.png";

		smoke = new Smoke();
		g_ParticleSystem()->spawn(smoke);
	}
}

void Bullet::update()
{
	position = position + velocity * g_Timer()->getFrameTime();
	if(type == BulletType::BULLET_Rocket)
	{
		smoke->pos = D3DXVECTOR3(position.x, position.y, 0);
	}
}

void Bullet::draw()
{
	texture.set();
	g_Renderer()->drawRect(position.x, position.y, 10, 10);
}

Bullet* Vehicle::fire(D3DXVECTOR2 dir)
{
	Bullet* bullet = new Bullet(bulletType);
	bullet->position = position;
	bullet->velocity = dir;
	return bullet;
}

void Vehicle::update()
{
	if(!group->started)
		return;

	D3DXVECTOR2 nextPos = group->nodes[currentNode + 1]->position;
	if(currentNode + 1 < group->nodes.size())
	{
		dir = nextPos - group->nodes[currentNode]->position;
	}
	position = position + dir * velocity * g_Timer()->getFrameTime();

	if(D3DXVec2Length(&(nextPos - position))) 
	{
		currentNode++;
	}
}

Map::Map() : cameraPosition(20,3,15)
{
	width = height = 0;
	nodeCount = vehicleCount = 0;
}

Map::~Map()
{
}

void Map::setupGroups()
{
	groupCount = 4; //0;
	D3DXVECTOR2 startPos = D3DXVECTOR2((g_Window()->getWidth() - GROUP_BUTTON_SIZE * groupCount) / 2, 
		g_Window()->getHeight() - GROUP_BUTTON_SIZE);
	float groupRay = GROUP_BUTTON_SIZE;
	for(int i = 0; i < groupCount; ++i)
	{
		groups[i].pos = startPos + D3DXVECTOR2((groupRay + 10) * i, 0);
	}

	groupCount = 4;
	int groupIndex = 0;
	for (int i=0; i < 16; ++i) {
		if (i > 3 && i % 4 == 0) groupIndex++;
		vehicles[i].group = &groups[groupIndex];
		vehicles[i].tile = Texture( i % 2 == 0 ? "tank_tile.png" : "smoketex.jpg" );
	}
	vehicleCount = 16;
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
				int index = (map->height - i - 1) * map->width + j;
				fread(bytes, 3, 1, file);
				std::swap(bytes[0], bytes[2]);
				if(bytes[0]>80 && bytes[1]>80 && bytes[2]>80)
					continue;

				if(bytes[0] > 80 && bytes[1] <= 80 && bytes[2] <= 80) {
					map->tiles[j][i].type = TILE_Street;
				}
				else if(bytes[0] <= 80 && bytes[1] <= 80 && bytes[2] > 80) {
					map->tiles[j][i].type = TILE_Water;
				}
				else if(bytes[0] <= 80 && bytes[1] <= 80 && bytes[2] <= 80) {
					map->tiles[j][i].type = TILE_Block;
					map->tiles[j][i].random = 1;// + 0.5 * (float)rand() / RAND_MAX;
				}
			}
			fread(bytes, pad-byteWidth, 1, file);
		}
	}

	fclose(file);

	map->load(name);

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


				// add node
				cur.node = &nodes[nodeCount++];
				cur.node->position = D3DXVECTOR2(j,i);
				cur.node->otherNodes.clear();
			}
			else if(cur.type == TILE_Block)
			{
				//cur.mesh = &g_Game->ap_b;
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

			if(lastNode)
			{
				lastNode->otherNodes.push_back(node);
				node->otherNodes.push_back(lastNode);
			}
			lastNode = node;
		}
	}
}

void Map::update()
{
	int dx, dy;
	int scroll;
	g_Input()->getMovement(dx, dy);
	g_Input()->getScroll(scroll);
	float sensitivity = 8;
	cameraPosition.x -= dx * sensitivity * g_Timer()->getFrameTime(); 
	cameraPosition.z += dy * sensitivity * g_Timer()->getFrameTime();

	if(scroll > 0)
		cameraPosition.y = max(cameraPosition.y - 3, 2);
	else if(scroll < 0)
		cameraPosition.y = min(cameraPosition.y + 3, 100);

	// start groups
	if((GetKeyState('Q') & 0x80) && !groups[0].started)
	{
		groups[0].started = true;
	}
	else if((GetKeyState('W') & 0x80) && !groups[1].started)
	{
		groups[1].started = true;
	}
	else if((GetKeyState('E') & 0x80) && !groups[2].started)
	{
		groups[2].started = true;
	}
	else if((GetKeyState('R') & 0x80) && !groups[3].started)
	{
		groups[3].started = true;
	}

	for(int i = 0; i < bullets.size(); ++i)
	{
		bullets[i]->update();
		// TODO: collisions
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

void Map::drawTiles()
{
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			Tile& tile = tiles[j][i];

			switch(tile.type)
			{
			case TILE_Street:
			case TILE_Water:
			case TILE_Grass:
			case TILE_Ignore:
				{
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
				getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, COUNT_OF(verts) - 2, verts, sizeof(verts[0]));
				}
				break;

			case TILE_Block:
				{
					if(!tile.mesh)
						continue;
				tile.mesh->draw(D3DXVECTOR3(j + 0.5f, 0, i + 0.5f), D3DXVECTOR3());
				}
				break;
			}
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
			
			line->DrawTransform(v, 2, trans, D3DCOLOR_ARGB(100,100,100,200));
		}
	}

	line->End();

	line->Release();
}

void Map::draw()
{
	getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128,128,128), 1.0f, 0);

	// setup projection matrix
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovRH(&proj, 90, 4.0f/3.0f, 1, 10000);
	getDevice()->SetTransform(D3DTS_PROJECTION, &proj);

	// setup view matrix
	D3DXMATRIX view;
	D3DXVECTOR3 at, up(0,0,1);
	at = cameraPosition;
	at.x += 0;
	//at.y += 1;
	at.y= 0;
	D3DXMatrixLookAtRH(&view, &cameraPosition, &at, &up);
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
	light.Position = cameraPosition;
	light.Ambient.r  = 0.4f;
	light.Ambient.g  = 0.4f;
	light.Ambient.b  = 0.4f;
	light.Ambient.a  = 0.4f;
	light.Diffuse.r  = 0.6f;
	light.Diffuse.g  = 0.6f;
	light.Diffuse.b  = 0.6f;
	light.Diffuse.a  = 0.6f;
	light.Attenuation1 = 0.4f;
	light.Range      = 100000.0f;
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(0.0f,-1.0,0.3);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	getDevice()->SetLight(0, &light);
	getDevice()->LightEnable(0, TRUE);
	getDevice()->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50,50,50));

	// draw map
	drawTiles();

	g_Game->tank1.draw(D3DXVECTOR3(10, 1, 15), D3DXVECTOR3());

	// setup drawing options
	getDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// draw nodes
	drawNodes(&matrix);

	// disable z
	getDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	g_Direct3D()->setViewMatrix();
	getDevice()->SetTexture(0, NULL);
	// draw groups buttons
	for(int i = 0; i < groupCount; ++i)
	{
		g_Renderer()->drawRectRHW(groups[i].pos.x - GROUP_BUTTON_SIZE / 2,
			groups[i].pos.y - GROUP_BUTTON_SIZE / 2, GROUP_BUTTON_SIZE, GROUP_BUTTON_SIZE, 
			groups[i].started ? D3DXCOLOR(0, 1, 0, 1) : D3DXCOLOR(1, 0, 0, 1));
	}
}
