#pragma once
#include "Common.h"

class Direct3D : public Singleton<Direct3D>
{
private:
	LPDIRECT3D9				d3D;
	LPDIRECT3DDEVICE9		d3dDevice;
	D3DPRESENT_PARAMETERS	d3dpp; 

	LPDIRECT3DQUERY9		eventQuery;

	int refreshRate;	
	void findBestRefreshRate();

public:
	Direct3D();
	~Direct3D();

	// inicjalizacja dx
	void initDirect3D(int bits, bool isFullscreen);
	void releaseDirect3D();
	void restoreDevice();

	void setDefaultSettings();		

	void setDisplayMode(int width, int height) 
	{
		d3dpp.BackBufferWidth  = width;
		d3dpp.BackBufferHeight = height;

		findBestRefreshRate();

		if( !d3dpp.Windowed )
			d3dpp.FullScreen_RefreshRateInHz = refreshRate;
		else
			d3dpp.FullScreen_RefreshRateInHz = 0;
	}

	DWORD checkRenderState(D3DRENDERSTATETYPE state_);
	void setRenderState(D3DRENDERSTATETYPE state, DWORD value);
	void enableAlphaBlending(bool state_);

	void processEventQuery();

	void onLostDevice();
	void onResetDevice();

	void reset() { d3dDevice->Reset(&d3dpp); }

	// pobieranie d3d device
	LPDIRECT3DDEVICE9 getDevice() { return d3dDevice; }
};
/*DefineAccessToSingleton(Direct3D);*/
inline Direct3D* g_Direct3D() { 
	return Direct3D::getSingletonPtr(); 
}

inline LPDIRECT3DDEVICE9 getDevice() {
	return g_Direct3D()->getDevice();
}

inline DWORD Direct3D::checkRenderState(D3DRENDERSTATETYPE state_) 
{
	DWORD value;
	d3dDevice->GetRenderState(state_, &value);
	return value;
}

inline void Direct3D::setRenderState(D3DRENDERSTATETYPE state, DWORD value) 
{
	if(checkRenderState(state) != value) {
		d3dDevice->SetRenderState(state, value);
	}
}

inline void Direct3D::enableAlphaBlending(bool state_) 
{ 
	setRenderState(D3DRS_ALPHABLENDENABLE, state_);

	if(!state_) return;

	if (checkRenderState(D3DRS_SRCBLEND) != D3DBLEND_SRCALPHA) {
		d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	}

	if (checkRenderState(D3DRS_DESTBLEND) != D3DBLEND_INVSRCALPHA) {
		d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}	
}