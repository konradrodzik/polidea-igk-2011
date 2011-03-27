#include "common.h"
#include "Window.h"

// for physics editing
std::vector<D3DXVECTOR2> points;
std::vector<std::vector<D3DXVECTOR2>> lines;
bool lmousebutton = true;
bool editorMode = false;
int mouseX=0, mouseY=0;
bool pause;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_Window()->WndProc(hwnd, uMsg, wParam, lParam);
}

Window::Window()
	: width(0), 
	height(0), 
	bits(0),
	isInit_(false), 
	isWindowed_(false), 
	isDone_(false)
{
}

Window::~Window()
{
}

bool Window::initApplication(const string& appName, 
	int width_, int height_, HINSTANCE hInstance_, bool isFullScreen)
{
	hInstance = hInstance_;
	strAppName = appName;

	RECT r = {0, 0, width_, height_};
	
	AdjustWindowRectEx(&r, isFullScreen ? WS_POPUP | WS_SYSMENU | WS_VISIBLE: WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU
						  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, FALSE, 0);

	width  = r.right - r.left;
	height = r.bottom - r.top;

	// rejestrowanie klasy okna
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW,
					::WndProc, 0L, 0L, 
					hInstance, LoadIcon(NULL, "hearfire.ico"), 0, (HBRUSH)COLOR_ACTIVEBORDER, NULL,
					strAppName.c_str(), NULL };

	if(!RegisterClassEx( &wc )) {
		return false;
	}

	// utworzenie okna
	hWnd = CreateWindowEx(NULL, strAppName.c_str(), strAppName.c_str(), isFullScreen ? WS_POPUP | WS_SYSMENU | WS_VISIBLE: WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU
						  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, width, height, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL)
		return false;


	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	RECT rect;
	GetClientRect(getHWND(), &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	//ShowCursor(false);

	clearKeyState();
	return true;
}

bool Window::shutdownApplication()
{	
	DestroyWindow(hWnd);
	UnregisterClass(strAppName.c_str(), hInstance);
	return true;
}

void Window::updateApplication()
{
	static MSG msg;
	if(PeekMessage(&msg, hWnd, NULL, NULL, PM_REMOVE))
	{
		if(msg.message == WM_QUIT)
			isDone_ = true;
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}	
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		isDone_ = true;
		PostQuitMessage(0);
		return 0;

	case WM_CHAR:
		charPressed = (char)wParam;		
		break;

	case WM_KEYUP:
		updateKeyState((int)wParam, false);
		break;

	case WM_KEYDOWN:
		updateKeyState((int)wParam, true);
		break;

 	case WM_SETCURSOR:
		SetCursor(LoadCursor(NULL, IDC_CROSS));
 		break;

	case WM_LBUTTONDOWN:
		
		if(lmousebutton && editorMode)
		{
			points.push_back(D3DXVECTOR2(g_Mouse()->getX(), g_Mouse()->getY()));
			lmousebutton = false;
		}
		
		break;

	case WM_LBUTTONUP:
		lmousebutton = true;
		break;

	case WM_MOUSEMOVE:
		{
			g_Mouse()->setPos((float)LOWORD(lParam), g_Window()->getHeight() - (float)HIWORD(lParam));
			mouseX = g_Mouse()->getX();
			mouseY = g_Mouse()->getY();
		}
	break;

	case WM_RBUTTONDOWN:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}
