#pragma once
#include "Common.h"

class Texture : public resource<Texture>
{
public:
	Texture() : pTexture(NULL) { 
		addResource(); 
	}
	Texture(const string& name_) : pTexture(NULL) { 
		addResource(); load(name_); 
	}
	virtual ~Texture() { removeResource(); }

	void load(const string& textureName_, int mipMapLevels = 1);
	void release();

	void set(int unit = 0) {
		getDevice()->SetTexture(unit, pTexture);
	}

	void operator= (const string& name_) { load(name_); }

	string getTextureName() const		{ return textureName;	}
	LPDIRECT3DTEXTURE9 getTexture() const	{ return pTexture;		}

private:
	string textureName;
	LPDIRECT3DTEXTURE9 pTexture;
};
