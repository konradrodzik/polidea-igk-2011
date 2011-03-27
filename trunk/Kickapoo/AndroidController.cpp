#include "Common.h"
#include "AndroidController.h"

DWORD WINAPI MyThreadFunction( LPVOID lpParam );

AndroidController::AndroidController(void)
{
	ListenSocket = INVALID_SOCKET;
	result = NULL;
	ptr = NULL;
	haveConnected = false;
	reset();
}

AndroidController::~AndroidController(void)
{
}

bool AndroidController::create(void)
{
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		return false;
	}

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return false;
	}

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MyThreadFunction,       // thread function name
            NULL,			        // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadId);			// returns the thread identifier
	return true;
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
	g_AndroidController()->ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	g_AndroidController()->ClientSocket = accept(g_AndroidController()->ListenSocket, NULL, NULL);
	if (g_AndroidController()->ClientSocket == INVALID_SOCKET) {
		closesocket(g_AndroidController()->ListenSocket);
		WSACleanup();
		return 1;
	}

	g_AndroidController()->haveConnected = true;

	#define DEFAULT_BUFLEN 512

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(g_AndroidController()->ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			g_AndroidController()->parse(recvbuf, recvbuflen);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(g_AndroidController()->ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);
    return 0; 
} 

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

// TODO: thread synchronization!
void AndroidController::parse(char* buffer, int len)
{
	string message(buffer, len);
	// message_ID param1 param2 ...
	vector<string> parts = split(message, ' ');

	if(parts[0] == "accel")
	{
		accelerator.x = atof(parts[1].c_str());
		accelerator.y = atof(parts[1].c_str());
		accelerator.z = atof(parts[1].c_str());
	}
	else if(parts[0] == "button")
	{
		int index = atoi(parts[1].c_str());
		if(index > 0 && index < 20)
		{
			buttons.button[index] = true;
		}
	}
}

void AndroidController::close(void)
{
	WSACleanup();
}