#pragma once
#include "Common.h"


class Window : public Singleton<Window>
{
public:
	Window();
	virtual ~Window();

	LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	bool initApplication(const string& appName, int width_, int height_, HINSTANCE hInstance, bool isFullScreen);
	bool shutdownApplication();

	void updateApplication();

	char getPressedChar() 
	{
		if (charPressed != '\0') 
		{
			char temp = charPressed;
			charPressed = '\0';
			return temp;
		}
		return '\0';
	}

	HDC&  getHDC()  { return hDC; }
	HWND& getHWND() { return hWnd; }
	HINSTANCE getInstance() { return hInstance; }

	int getWidth()  { return width;  }
	int getHeight() { return height; }
	int  getBits()	{ return bits;   }

	
	void setWindowSize(int width_, int height_)	
	{
		width = width_; height = height_;
		MoveWindow(hWnd, 0, 0, width, height, true);
		RECT rect;
		GetClientRect(getHWND(), &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	bool isDone()			{ return isDone_; }
	void exitApplication()	{ isDone_ = true; }

	bool isInit()	  { return isInit_;		}
	bool isWindowed() { return isWindowed_; }

	void checkDeviceCaps();
	
	bool keyDown(int key)	{ return  keys[key]; }
	bool keyUp(int key)		{ return !keys[key]; }
	bool keyPressed(int key)	{ return (keys[key] && tempKeys[key]); }

protected:
	string strAppName;

	HWND      hWnd;
	HINSTANCE hInstance;
	HDC		  hDC;

	// rozmiary okna
	int width;
	int height;
	int bits;
	
	bool keys[256];
	bool tempKeys[256];

	// z procedury okienkowej
	char charPressed;

	// zmienne wykonania
	bool isInit_;
	bool isWindowed_;
	bool isDone_;

	void updateKeyState(int key, bool state)	{
		tempKeys[key] = (state != keys[key]);
		keys[key]     = state;
	}

	void clearKeyState() {
		for (int i = 0; i < 256; i++)
			keys[i] = tempKeys[i] = false;
	}
};
// DefineAccessToSingleton(Window);

inline Window* g_Window() { return Window::getSingletonPtr(); }