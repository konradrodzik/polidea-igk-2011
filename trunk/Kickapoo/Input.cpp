#include "Common.h"
#include "Input.h"

#define check(a) a

void Input::init(HINSTANCE appInstance, bool isExclusive)
{
	if ( FAILED( DirectInput8Create(appInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&pDI, NULL) ) )
	{
		return;
	}

	// init myszki
	check( pDI->CreateDevice(GUID_SysMouse, &pDIDev, NULL) );
	check( pDIDev->SetDataFormat(&c_dfDIMouse) );

	DWORD flags;
	if (isExclusive)   
		flags = DISCL_FOREGROUND | DISCL_EXCLUSIVE | DISCL_NOWINKEY;
	else			  
		flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	check( pDIDev->SetCooperativeLevel(g_Window()->getHWND(), flags ));
	check( pDIDev->Acquire() );
	
	check( pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &state) );

	// wyczyszczenie stanu klawiszy
	for(int i = 0; i < 256; i++) {
		keys[i] = tempKeys[i] = false;
	}
}

void Input::release()
{
	unacquire();

	if (pDIDev)
	{
		pDIDev->Unacquire();
		pDIDev->Release();
	}

	if ( FAILED( pDI->Release() ) )
		return;
}

bool Input::acquire()
{
	return !FAILED( pDIDev->Acquire() );
}

bool Input::unacquire()
{
	return !FAILED( pDIDev->Unacquire() );
}

void Input::update()
{
	// update klawiszy
	for (int i = 0; i < 256; i++) 
	{
		bool state  = static_cast<bool>(GetAsyncKeyState(i));
		tempKeys[i] = (state != keys[i]);
		keys[i]     = state;
	}

	// update myszki
	if ( FAILED( pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &state) ) )
	{
		if ( FAILED( pDIDev->Acquire() ) )
			return;
		if ( FAILED( pDIDev->GetDeviceState(sizeof(DIMOUSESTATE), &state) ) )
			return;
	}
}
