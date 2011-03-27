#pragma once
#include "Common.h"

class RenderToTexture
{
public:
	RenderToTexture(void)	{}
	~RenderToTexture(void)	{}

	void init(int screenWidth_, int screenHeight_);
	void release();

	void beginRenderToTexture();
	void endRenderToTexture();

	void draw(float x, float y, float width, float height);

	void onLostDevice();
	void onResetDevice();

	LPDIRECT3DTEXTURE9* getTexture() { return &renderTexture; }

private:
	int width, height;

	LPDIRECT3DTEXTURE9 renderTexture;

	LPDIRECT3DSURFACE9 renderSurface;
	LPDIRECT3DSURFACE9 depthSurface;

	LPDIRECT3DSURFACE9 backBuffer;
	LPDIRECT3DSURFACE9 depthBuffer;
};