#include "Common.h"
#include "Mesh.h"

const char* basename(const char* p);

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
		fail:
		pMesh = NULL;
		textures.clear();
		materials.clear();
		return;
	}

	D3DXComputeNormals(pMesh, NULL);

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();
	materials.assign(d3dxMaterials, d3dxMaterials+numMaterials);

	textures.clear();
	textures.resize(numMaterials);

	for(int i = 0; i < numMaterials; ++i)
	{
		if(materials[i].pTextureFilename)
			textures[i].load(string("models/") + materials[i].pTextureFilename);
		else
			textures[i].load(string(basename(MeshName.c_str())) + ".bmp");
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
	//getDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
	getDevice()->SetTransform(D3DTS_WORLD, &trans);

	D3DXVECTOR3 up(0,1,0);
	D3DXVECTOR3 tan;
	D3DXVec3Cross(&tan, &dir, &up);
	D3DXMatrixIdentity(&trans);

	float angle = acosf(dir.x);
	if(dir.x == 0 || tanf(dir.z/dir.x) < 0)
		angle += 3.14;
	D3DXMatrixRotationY(&trans, angle);
	getDevice()->SetTransform(D3DTS_WORLD2, &trans);

	for(DWORD i=0; i<materials.size(); i++)
	{
		textures[i].set(0);
		pMesh->DrawSubset(i);
	}

	D3DXMatrixIdentity(&trans);
	getDevice()->SetTransform(D3DTS_WORLD, &trans);
	getDevice()->SetTransform(D3DTS_WORLD1, &trans);
}