#include "Common.h"
#include "Particles.h"
#include <math.h>

#define FVF (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE)

void ParticleEffect::draw(VertexBuffer* vb, BufferChunk& chunk)
{
	texture.set();
	vb->pushData(sizeof(particle_vertex) * vert.size(), vert.data(), chunk);

	getDevice()->SetFVF(FVF);
	getDevice()->SetStreamSource(0, vb->getBuffer(), chunk.offset, sizeof(particle_vertex));
	getDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, vert.size());
}

ParticleSystem::ParticleSystem()
	: instances()
{
}

ParticleSystem::~ParticleSystem()
{
	for(std::vector<ParticleEffect*>::iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		ParticleEffect* p = *it;
		delete p;
	}
}

void ParticleSystem::updateParticles()
{
	if(instances.empty())
		return;

	for(int i = 0; i < instances.size(); )
	{
		if(instances[i]->update())
		{
			++i;
		} else {
			ParticleEffect* p = instances[i];
			delete p;
			instances.erase(instances.begin() + i);
		}
	}
}

void ParticleSystem::renderParticles()
{
	if(instances.empty())
		return;

	g_Direct3D()->getDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	g_Direct3D()->getDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	DWORD value[6];
	getDevice()->GetRenderState(D3DRS_ALPHABLENDENABLE, &value[0]);
	getDevice()->GetRenderState(D3DRS_SRCBLEND, &value[1]);
	getDevice()->GetRenderState(D3DRS_DESTBLEND, &value[2]);
	getDevice()->GetTextureStageState(0, D3DTSS_ALPHAOP, &value[3]);
	getDevice()->GetTextureStageState(0, D3DTSS_ALPHAARG1, &value[4]);
	getDevice()->GetTextureStageState(0, D3DTSS_ALPHAARG2, &value[5]);
	
	getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	getDevice()->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	getDevice()->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	g_Renderer()->setIdentity();
	for(std::vector<ParticleEffect*>::const_iterator it = instances.begin() ; it != instances.end() ; ++it)
	{
		(*it)->draw(vb, chunk);
	}

	getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, value[0]);
	getDevice()->SetRenderState(D3DRS_SRCBLEND, value[1]);
	getDevice()->SetRenderState(D3DRS_DESTBLEND, value[2]);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, value[3]);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, value[4]);
	getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, value[5]);
}