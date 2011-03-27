#include "Common.h"
#include "Texture.h"

void Texture::load(const string& textureName_, int mipMapLevels)
{
	textureName = textureName_;
	release();

	if(D3DXCreateTextureFromFileEx(
		getDevice(),
		textureName_.c_str(),
		D3DX_DEFAULT_NONPOW2, 
		D3DX_DEFAULT_NONPOW2,	
		mipMapLevels, 
		0,
		D3DFMT_UNKNOWN,	
		D3DPOOL_MANAGED, 
		mipMapLevels == -1 ? D3DX_FILTER_NONE : D3DX_DEFAULT, 
		D3DX_DEFAULT,
		0, 
		NULL, 
		NULL, &pTexture) != D3D_OK ) 
	{
		pTexture = NULL;
	}
}

void Texture::release()
{		
	if (pTexture)
	{
		pTexture->Release();
		pTexture = NULL;
	}
}