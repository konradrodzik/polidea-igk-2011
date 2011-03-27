#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Common.h"
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_PORT "27015"

struct Accelerator
{
	float x;
	float y;
	float z;
};

struct Buttons
{
	bool button[20];
};

class AndroidController : public Singleton<AndroidController>
{
public:
	AndroidController(void);
	~AndroidController(void);

	bool create();
	void close();

	Accelerator accelerator;
	Buttons buttons;

	void parse(char* buffer, int len);

	/** This should be callend on the end of every frame */
	void reset()
	{
		ZeroMemory(&accelerator, sizeof (accelerator));
		ZeroMemory(&buttons, sizeof (buttons));
	}

	WSADATA wsaData;
	addrinfo *result, *ptr, hints;

	SOCKET ListenSocket;
	SOCKET ClientSocket;
	HANDLE hThread;
	DWORD dwThreadId;

	bool haveConnected;
};
DefineAccessToSingleton(AndroidController)