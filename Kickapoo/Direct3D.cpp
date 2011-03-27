#include "common.h"
#include "Direct3D.h"
#include "Window.h"

Direct3D::Direct3D() : d3D(NULL), d3dDevice(NULL), eventQuery(NULL)	{}
Direct3D::~Direct3D()	{}	

void Direct3D::initDirect3D(int bits, bool isFullscreen)
{
	if( NULL == ( d3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) {
		return;
	}

	D3DDISPLAYMODE displayMode;
	d3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	findBestRefreshRate();

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	RECT rect;
	GetClientRect(g_Window()->getHWND(), &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	d3dpp.Windowed = !isFullscreen;		
	if(isFullscreen) {
		d3dpp.BackBufferWidth  = width;
		d3dpp.BackBufferHeight = height;
	}

	d3dpp.BackBufferCount  = 1;
	d3dpp.BackBufferFormat = (bits == 32) ? displayMode.Format : D3DFMT_R5G6B5;
	
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;

	if( !d3dpp.Windowed )
		d3dpp.FullScreen_RefreshRateInHz = refreshRate;
	else
		d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; 


	if(FAILED(d3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_Window()->getHWND(),
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								 &d3dpp, &d3dDevice ))) 
	{
		return;
	}

	d3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &eventQuery);

	setDefaultSettings();
}

void Direct3D::findBestRefreshRate()
{
	D3DDISPLAYMODE displayMode;
	d3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	int dispalyModeCount = d3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, displayMode.Format);
	D3DDISPLAYMODE* pMode = new D3DDISPLAYMODE[ dispalyModeCount ]; 
	
	d3D->EnumAdapterModes(D3DADAPTER_DEFAULT, displayMode.Format, dispalyModeCount - 1, pMode);

	RECT rect;
	GetClientRect(g_Window()->getHWND(), &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	int currentRefreshRate = 0;
	for (int i = 0; i < dispalyModeCount; i++)
	{
		if (pMode[i].Width  == width  && 
			pMode[i].Height == height && 
			currentRefreshRate < pMode[i].RefreshRate)
		{
			currentRefreshRate = pMode[i].RefreshRate;
		}
	}

	refreshRate = currentRefreshRate;

	if(pMode)
		delete pMode;
}

void Direct3D::releaseDirect3D()
{
	if(eventQuery)
		eventQuery->Release();

	if( d3dDevice != NULL) 
		d3dDevice->Release();
	
	if( d3D != NULL)
		d3D->Release();
}

//DWORD FloatToDWORD( float f ) { return *((DWORD*)&f); }

void Direct3D::setDefaultSettings()
{
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);		
	d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

	d3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

	d3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // D3DTEXF_POINT
	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // D3DTEXF_LINEAR

	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	d3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FloatToDWORD(0.01f) );
	d3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FloatToDWORD(128.0f) );

	RECT rect;
	GetClientRect(g_Window()->getHWND(), &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	D3DXMatrixOrthoLH(&matrix, width, height, -1, 1);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &matrix);

	D3DXMatrixTranslation(&matrix, -width / 2.0f, -height / 2.0f, 0);
	d3dDevice->SetTransform(D3DTS_VIEW, &matrix);
}

void Direct3D::setViewMatrix()
{
	d3dDevice->SetTransform(D3DTS_VIEW, &matrix);
}

void Direct3D::restoreDevice()
{
	HRESULT result = d3dDevice->TestCooperativeLevel();

	while(result == D3DERR_DEVICELOST) 
	{
		while(result != D3DERR_DEVICENOTRESET) 
		{
			MSG message;

			PeekMessage(&message, 0, 0, 0, PM_REMOVE);

			TranslateMessage(&message);
			DispatchMessage(&message);

			result = d3dDevice->TestCooperativeLevel();
		}			
		if ( FAILED( d3dDevice->Reset(&d3dpp) ) )
			result = D3DERR_DEVICELOST;
	}
}

void Direct3D::processEventQuery()
{
	eventQuery->Issue(D3DISSUE_END);
	while(S_FALSE == eventQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ))
		;
}

void Direct3D::onLostDevice()
{
	if(eventQuery)
		eventQuery->Release();
}

void Direct3D::onResetDevice()
{	
	d3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &eventQuery);
}