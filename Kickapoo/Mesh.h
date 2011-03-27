#pragma once
#include "Common.h"
#include "Texture.h"

class Mesh : public resource<Mesh>
{
public:
	Mesh() : pMesh(NULL),scale(1,1,1) { 
		addResource(); 
	}
	Mesh(const string& name_) : pMesh(NULL),scale(1,1,1) { 
		addResource(); load(name_); 
	}
	virtual ~Mesh() { removeResource(); }

	void load(const string& MeshName_);
	void release();

	void draw(const D3DXVECTOR3& position, const D3DXVECTOR3& dir);

	void operator= (const string& name_) { load(name_); }

	string getMeshName() const		{ return MeshName;	}
	LPD3DXMESH getMesh() const	{ return pMesh;		}
	void setScale(float x, float y, float z) { scale.x = x; scale.y = y; scale.z = z; }

private:
	string MeshName;
	LPD3DXMESH pMesh;
	vector<D3DXMATERIAL> materials;
	vector<Texture> textures;
	D3DXVECTOR3 scale;
};
