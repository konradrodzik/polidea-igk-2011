#pragma once
#include "Common.h"

#define FVF_TEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct BufferChunk
{
	DWORD offset;
	DWORD size;

	BufferChunk() : offset(0), size(0) {};
};

class VertexBuffer
{
private:
	LPDIRECT3DVERTEXBUFFER9 pBuffer;

	DWORD offset;
    DWORD bufferSize;

public:
	VertexBuffer(DWORD Size) : offset(0) { bufferSize = Size; }
	~VertexBuffer() {}

	void create();		
	void release();

	void pushData(DWORD size, void* data, BufferChunk& chunk);
	LPDIRECT3DVERTEXBUFFER9 getBuffer() { return pBuffer; }
};

struct vertex
{
	D3DXVECTOR3 pos;
	D3DCOLOR color;
	float tu, tv;
};

class Renderer : public Singleton<Renderer>
{
private:
	VertexBuffer* vb;
	BufferChunk chunk;

public:
	Renderer()	{}
	~Renderer() { if(vb) delete vb; }

	void create();
	void release();

	void onLostDevice();
	void onResetDevice();

	void setIdentity();
	void setRotation(float x, float y, float angle);

	void drawLine(float sx, float sy, float ex, float ey, float size = 1, D3DCOLOR color = 0xFFFFFFFF);

	void drawRect(float x, float y, float width, float height, D3DCOLOR color = 0xFFFFFFFF);
	void drawRect(float x, float y, float width, float height, 
		float u1, float v1, float u2, float v2, D3DCOLOR color = 0xFFFFFFFF);

	void drawRects(const std::vector<D3DXVECTOR2> * positions, const std::vector<D3DXVECTOR2> * sizes, const std::vector<D3DCOLOR> * colors, int count);

	void drawRect(
		float x1, float y1, float u1, float v1, 
		float x2, float y2, float u2, float v2, 
		float x3, float y3, float u3, float v3,
		float x4, float y4, float u4, float v4,
		D3DCOLOR color = 0xFFFFFFFF);

	void drawRotatedRect(float x, float y, float w, float h, D3DXVECTOR2& direction, D3DCOLOR color = 0xFFFFFFFF);
};
DefineAccessToSingleton(Renderer);

inline void TransformVertex(D3DXMATRIX* matrix, vertex* vertex_out, vertex* vertex_in, int count, int stride)
{
	for (int i = 0; i < count; i++)	{
		D3DXVec3TransformCoord(&vertex_out[i].pos, &vertex_in[i].pos, matrix);
	}
}
