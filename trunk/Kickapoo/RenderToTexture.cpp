#include "Common.h"
#include "RenderToTexture.h"

void RenderToTexture::init(int screenWidth_, int screenHeight_)
{
	width  = screenWidth_;
	height = screenHeight_;

	D3DXCreateTexture(getDevice(), width, height, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &renderTexture);

	getDevice()->CreateDepthStencilSurface(width, height,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE, 0, false,
		&depthSurface, NULL);

	getDevice()->GetRenderTarget(0, &backBuffer);
	getDevice()->GetDepthStencilSurface(&depthBuffer);
	
	renderTexture->GetSurfaceLevel(0, &renderSurface);
}

void RenderToTexture::release()
{
	if(renderSurface)
		renderSurface->Release();
	if(depthSurface)
		depthSurface->Release();

	if(renderTexture)
		renderTexture->Release();
	
	if(backBuffer)
		backBuffer->Release();
	if(depthBuffer)
		depthBuffer->Release();
}

void RenderToTexture::beginRenderToTexture()
{
	getDevice()->SetRenderTarget(0, renderSurface);
	getDevice()->SetDepthStencilSurface(depthSurface);

	getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
}

void RenderToTexture::endRenderToTexture()
{
	getDevice()->SetRenderTarget(0, backBuffer);
	getDevice()->SetDepthStencilSurface(depthBuffer);		
}

void RenderToTexture::draw(float x, float y, float width, float height)
{
	g_Direct3D()->getDevice()->SetTexture(0, renderTexture);
	g_Renderer()->drawRect(x, y, width, height);
}

void RenderToTexture::onLostDevice()
{
	release();
}

void RenderToTexture::onResetDevice()
{
	init(width, height);
}
