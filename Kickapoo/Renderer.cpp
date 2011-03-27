#include "Common.h"
#include "Renderer.h"

void VertexBuffer::create()
{
	getDevice()->CreateVertexBuffer(bufferSize, 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
		0, 
		D3DPOOL_DEFAULT, 
		&pBuffer, 
		NULL);
}

void VertexBuffer::pushData(DWORD size, void *data, BufferChunk& chunk)
{
	assert(pBuffer != NULL, "pBuffer == NULL");
	void* verticles;
	
	if((size + offset) >= bufferSize)
	{
		offset = 0;
		pBuffer->Lock(offset,size,(void**)&verticles,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
	}
	else
	{
		pBuffer->Lock(offset,size,(void**)&verticles, D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK);
	}

	memcpy(verticles,data,size);

	pBuffer->Unlock();

	chunk.offset = offset;
	chunk.size = size;

	offset += size;
}

void VertexBuffer::release()
{
	if(pBuffer != NULL)
		pBuffer->Release();		
}

//////////////////////////////////////////////////////////////////////
void Renderer::create()
{
	vb = new VertexBuffer(32768 * sizeof(vertex));	// 2^15
	vb->create();
}

void Renderer::release()
{
	vb->release();
}

void Renderer::onLostDevice()
{
	vb->release();
}

void Renderer::onResetDevice()
{
	vb->create();
}

void Renderer::setIdentity()
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	getDevice()->SetTransform(D3DTS_WORLD, &identity);
}

void Renderer::setRotation(float x, float y, float angle)
{
	D3DXMATRIX result, matrix;

	D3DXMatrixTranslation(&result, -x, -y, 0);
	D3DXMatrixRotationZ(&matrix, angle);
	D3DXMatrixMultiply(&result, &result, &matrix);
	
	D3DXMatrixTranslation(&matrix, x, y, 0);
	D3DXMatrixMultiply(&result, &result, &matrix);

	getDevice()->SetTransform(D3DTS_WORLD, &result);
}

void Renderer::drawLine(float sx, float sy, float ex, float ey, float size, D3DCOLOR color)
{
	D3DXVECTOR2 p(sx - ex, sy - ey);
	D3DXVECTOR2 right(-sy + ey, sx - ex);
	D3DXVec2Normalize(&right, &right);

	getDevice()->SetTexture(0, 0);
	drawRect(
		sx + right.x * size, sy + right.y * size, 0, 0, 
		ex + right.x * size, ey + right.y * size, 0, 0,
		ex - right.x * size, ey - right.y * size, 0, 0,
		sx - right.x * size, sy - right.y * size, 0, 0, color);
}

void Renderer::drawRect(float x, float y, float width, float height, D3DCOLOR color)
{
	vertex v[4];
	
	v[0].pos.x = x;
	v[0].pos.y = y;
	v[0].pos.z = 0;
//	v[0].rhw = 1.0f;
	v[0].color = color;
	v[0].tu = 0.0f;
	v[0].tv = 1.0f;

	v[1].pos.x = x;
	v[1].pos.y = y + height;
	v[1].pos.z = 0;
//	v[1].rhw = 1.0f;
	v[1].color = color;
	v[1].tu = 0.0f;
	v[1].tv = 0.0f;		

	v[2].pos.x = x + width;
	v[2].pos.y = y;
	v[2].pos.z = 0;
//	v[2].rhw = 1.0f;
	v[2].color = color;
	v[2].tu = 1.0f;
	v[2].tv = 1.0f;		

	v[3].pos.x = x + width;
	v[3].pos.y = y + height;
	v[3].pos.z = 0;
//	v[3].rhw = 1.0f;
	v[3].color = color;
	v[3].tu = 1.0f;
	v[3].tv = 0.0f;

	vb->pushData(sizeof(v), v, chunk);

	getDevice()->SetFVF(FVF_TEX);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(vertex));
	getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void Renderer::drawRects(const std::vector<D3DXVECTOR2> * positions, const std::vector<D3DXVECTOR2> * sizes, const std::vector<D3DCOLOR> * colors, int count)
{
	vertex * v;
	static std::vector<vertex> vertices;

	vertices.resize(count * 6);
	v = &vertices[0];

	for(int i = 0 ; i < count ; ++i)
	{
		float x = positions->at(i).x;
		float y = positions->at(i).y;
		float width = sizes->at(i).x;
		float height = sizes->at(i).y;
		D3DCOLOR color = colors->at(i);

		v[0].pos.x = x;
		v[0].pos.y = y;
		v[0].pos.z = 0;
		//	v[0].rhw = 1.0f;
		v[0].color = color;
		v[0].tu = 0.0f;
		v[0].tv = 1.0f;

		v[1].pos.x = x;
		v[1].pos.y = y + height;
		v[1].pos.z = 0;
		//	v[1].rhw = 1.0f;
		v[1].color = color;
		v[1].tu = 0.0f;
		v[1].tv = 0.0f;		

		v[2].pos.x = x + width;
		v[2].pos.y = y;
		v[2].pos.z = 0;
		//	v[2].rhw = 1.0f;
		v[2].color = color;
		v[2].tu = 1.0f;
		v[2].tv = 1.0f;		
	
		v[3].pos.x = x;
		v[3].pos.y = y+height;
		v[3].pos.z = 0;
		//	v[3].rhw = 1.0f;
		v[3].color = color;
		v[3].tu = 0.0f;
		v[3].tv = 1.0f;

		v[4].pos.x = x + width;
		v[4].pos.y = y;
		v[4].pos.z = 0;
		//	v[4].rhw = 1.0f;
		v[4].color = color;
		v[4].tu = 1.0f;
		v[4].tv = 1.0f;		

		v[5].pos.x = x + width;
		v[5].pos.y = y + height;
		v[5].pos.z = 0;
		//	v[5].rhw = 1.0f;
		v[5].color = color;
		v[5].tu = 1.0f;
		v[5].tv = 0.0f;

		v += 6;
	}

	vb->pushData(sizeof(vertex) * vertices.size(), &vertices[0], chunk);

	getDevice()->SetFVF(FVF_TEX);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(vertex));
	getDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2*count);
}

void Renderer::drawRect(float x, float y, float width, float height, 
	float u1, float v1, float u2, float v2, D3DCOLOR color)
{
	vertex v[4];

	v[0].pos.x = x;
	v[0].pos.y = y;
	v[0].pos.z = 0;
//	v[0].rhw = 1.0f;
	v[0].color = color;
	v[0].tu = u1;
	v[0].tv = v2;

	v[1].pos.x = x;
	v[1].pos.y = y + height;
	v[1].pos.z = 0;
//	v[1].rhw = 1.0f;
	v[1].color = color;
	v[1].tu = u1;
	v[1].tv = v1;		

	v[2].pos.x = x + width;
	v[2].pos.y = y;
	v[2].pos.z = 0;
//	v[2].rhw = 1.0f;
	v[2].color = color;
	v[2].tu = u2;
	v[2].tv = v2;		

	v[3].pos.x = x + width;
	v[3].pos.y = y + height;
	v[3].pos.z = 0;
//	v[3].rhw = 1.0f;
	v[3].color = color;
	v[3].tu = u2;
	v[3].tv = v1;

	vb->pushData(sizeof(v), v, chunk);

	getDevice()->SetFVF(FVF_TEX);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(vertex));
	getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void Renderer::drawRect(
	float x1, float y1, float u1, float v1, 
	float x2, float y2, float u2, float v2, 
	float x3, float y3, float u3, float v3,
	float x4, float y4, float u4, float v4,
	D3DCOLOR color)
{
	vertex v[4];

	v[0].pos.x = x1;
	v[0].pos.y = y1;
	v[0].pos.z = 0;
//	v[0].rhw = 1.0f;
	v[0].color = color;
	v[0].tu = u1;
	v[0].tv = v1;

	v[1].pos.x = x2;
	v[1].pos.y = y2;
	v[1].pos.z = 0;
//	v[1].rhw = 1.0f;
	v[1].color = color;
	v[1].tu = u2;
	v[1].tv = v2;	

	v[2].pos.x = x4;
	v[2].pos.y = y4;
	v[2].pos.z = 0;
//	v[2].rhw = 1.0f;
	v[2].color = color;
	v[2].tu = u4;
	v[2].tv = v4;

	v[3].pos.x = x3;
	v[3].pos.y = y3;
	v[3].pos.z = 0;
//	v[3].rhw = 1.0f;
	v[3].color = color;
	v[3].tu = u3;
	v[3].tv = v3;

	vb->pushData(sizeof(v), v, chunk);

	getDevice()->SetFVF(FVF_TEX);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(vertex));
	getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void Renderer::drawRotatedRect(float x, float y, float w, float h, D3DXVECTOR2& direction, D3DCOLOR color)
{
	D3DXVECTOR2 norm;
	D3DXVec2Normalize(&norm, &direction);
	D3DXVECTOR2 tan(-norm.y, norm.x);
	
	tan *= w/2;
	norm *= h/2;

	vertex v[4];

	v[1].pos.x = x - tan.x - norm.x;
	v[1].pos.y = y - tan.y - norm.y;
	v[1].pos.z = 0;
	//	v[0].rhw = 1.0f;
	v[1].color = color;
	v[1].tu = 0.0f;
	v[1].tv = 1.0f;

	v[0].pos.x = x + tan.x - norm.x;
	v[0].pos.y = y + tan.y - norm.y;
	v[0].pos.z = 0;
	//	v[1].rhw = 1.0f;
	v[0].color = color;
	v[0].tu = 1.0f;
	v[0].tv = 1.0f;	

	v[2].pos.x = x + tan.x + norm.x;
	v[2].pos.y = y + tan.y + norm.y;
	v[2].pos.z = 0;
	//	v[2].rhw = 1.0f;
	v[2].color = color;
	v[2].tu = 1.0f;
	v[2].tv = 0.0f;

	v[3].pos.x = x - tan.x + norm.x;
	v[3].pos.y = y - tan.y + norm.y;
	v[3].pos.z = 0;
	//	v[3].rhw = 1.0f;
	v[3].color = color;
	v[3].tu = 0.0f;
	v[3].tv = 0.0f;

	vb->pushData(sizeof(v), v, chunk);

	getDevice()->SetFVF(FVF_TEX);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(vertex));
	getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
