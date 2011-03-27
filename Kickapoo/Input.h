#pragma once
#include "Common.h"

class Input : public Singleton<Input>
{
public:
	Input()				{}
	virtual ~Input()	{}

	void init(HINSTANCE appInstance, bool isExclusive = true);
	void release();

	bool keyDown(int key)    { return  keys[key];  }
	bool keyUp(int key)      { return (!keys[key] && tempKeys[key]); }
	bool keyPressed(int key) { return (keys[key]  && tempKeys[key]); }

	char getChar() 
	{
		for (int i = ' '; i < '~'; i++) 
		{
			if (keyPressed(i))
				return (char)i;
		}
		return '\0';
	}

	void update();

	bool  buttonDown(int button) { 
		return (state.rgbButtons[button] & 0x80) ? true : false; 
	}
	
	bool  buttonClicked(int button) 
	{
		static bool temp = false;
		
		bool result = false;
		bool buttonPressed = (state.rgbButtons[button] & 0x80) ? true : false;
		
		if (buttonPressed && !temp) {
			result = true;
		}
		if (buttonPressed) {
			temp = true;
		}
		else 
			temp = false;	

		return result; 
	}
	
	bool  buttonUp(int button)  { 
		return (state.rgbButtons[button] & 0x80) ? false : true; 
	}

	void  getMovement(int &dx, int &dy) { 
		dx = state.lX; 
		dy = state.lY; 
	}
	void  getScroll(int &whell) { 
		whell = state.lZ;
	}
	
	bool  acquire();
	bool  unacquire();

private:
	bool keys[256];
	bool tempKeys[256];


	LPDIRECTINPUT8 pDI;
	LPDIRECTINPUTDEVICE8  pDIDev;
	DIMOUSESTATE          state;
};
DefineAccessToSingleton(Input);