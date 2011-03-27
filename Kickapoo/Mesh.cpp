#include "Common.h"
#include "Mesh.h"

void Mesh::load(const string& meshName_)
{
	MeshName = meshName_;

	LPD3DXBUFFER materialBuffer;
	DWORD numMaterials;            // Note: DWORD is a typedef for unsigned long

	// Load the mesh from the specified file
	HRESULT hr=D3DXLoadMeshFromX(MeshName.c_str(), D3DXMESH_SYSTEMMEM, 
								 getDevice(), NULL, 
								 &materialBuffer,NULL, &numMaterials, 
								 &pMesh );
	if(FAILED(hr))
	{
		pMesh = NULL;
		textures.clear();
		materials.clear();
		return;
	}

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();
	materials.assign(d3dxMaterials, d3dxMaterials+numMaterials);

	textures.clear();
	textures.resize(numMaterials);

	for(int i = 0; i < numMaterials; ++i)
	{
		if(materials[i].pTextureFilename)
			textures[i].load(materials[i].pTextureFilename);
	}

	materialBuffer->Release();
}

void Mesh::release()
{		
	if (pMesh)
	{
		pMesh->Release();
		pMesh = NULL;
		materials.clear();
		textures.clear();
	}
}

void Mesh::draw(const D3DXVECTOR3& position, const D3DXVECTOR3& dir)
{
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
	trans.m[0][0] = scale.x;
	trans.m[1][1] = scale.y;
	trans.m[2][2] = scale.z;
	getDevice()->SetTransform(D3DTS_WORLD, &trans);

	for(DWORD i=0; i<materials.size(); i++)
	{
		textures[i].set(0);
		pMesh->DrawSubset(i);
	}

	D3DXMatrixIdentity(&trans);
	getDevice()->SetTransform(D3DTS_WORLD, &trans);
}