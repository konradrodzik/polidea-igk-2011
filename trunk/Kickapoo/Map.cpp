#include "Common.h"

Map::Map() : cameraPosition(25, 14, 3)
{
	width = height = 0;
	nodeCount = vehicleCount = 0;
	groupCount = 0;
}

Map::~Map()
{
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

	map->finalize();
	return map;
}

void Map::finalize()
{	
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
				if(left.type == right.type && left.type == TILE_Street)
				{
					if(up.type == down.type && down.type == TILE_Street)
					{
						cur.texture = &g_Game->street_cross;
						continue;
					}

					cur.texture = &g_Game->street;
					continue;
				}

				if(up.type == down.type && down.type == TILE_Street)
				{
					cur.texture = &g_Game->street;
					cur.offset = 2;
					continue;
				}

				if(left.type == up.type && left.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 1;
					continue;
				}

				if(left.type == down.type && left.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 0;
					continue;
				}

				if(right.type == down.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 3;
					continue;
				}

				if(right.type == up.type && right.type == TILE_Street)
				{
					cur.texture = &g_Game->street_corner;
					cur.offset = 2;
					continue;
				}
			}
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
	cameraPosition.y += dy * sensitivity * g_Timer()->getFrameTime();

	if(scroll > 0)
		cameraPosition.z = max(cameraPosition.z - 3, 2);
	else if(scroll < 0)
		cameraPosition.z = min(cameraPosition.z + 3, 100);
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
	{0,1,0,  0,0,1,  0,1},
	{1,1,0,  0,0,1,  1,1},
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
				{
					if(!tile.texture)
						continue;
				Vertex verts[COUNT_OF(RectVerts)];
				for(int k = 0; k < COUNT_OF(RectVerts); ++k)
				{
					verts[k] = RectVerts[k];
					verts[k].xyz[0] += j;
					verts[k].xyz[1] += i;
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
				Vertex verts[COUNT_OF(BoxVerts)];
				for(int k = 0; k < COUNT_OF(BoxVerts); ++k)
				{
					verts[k] = BoxVerts[k]; 
					verts[k].xyz[0] += j;
					verts[k].xyz[1] += i;
					verts[k].xyz[2] *= tile.random;
				}
				getDevice()->SetFVF(Vertex::FVF);
				getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, COUNT_OF(verts) / 3, verts, sizeof(verts[0]));
				}
				break;
			}
		}
	}
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
	D3DXVECTOR3 at, up(0,1,0);
	at = cameraPosition;
	at.x += 0;
	//at.y += 1;
	at.z = 0;
	D3DXMatrixLookAtRH(&view, &cameraPosition, &at, &up);
	getDevice()->SetTransform(D3DTS_VIEW, &view);

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
	vecDir = D3DXVECTOR3(0.0f,0.3,-1.0);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	getDevice()->SetLight(0, &light);
	getDevice()->LightEnable(0, TRUE);
	getDevice()->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50,50,50));

	drawTiles();

	// draw other objects

	// setup drawing options
	getDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	getDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}
