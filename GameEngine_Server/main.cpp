// WinSock2 Windows Sockets
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <conio.h>
#include <chrono>
#include <assert.h>

#include <map>
#include <vector>

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#include "ServerHelper.h"
int main(int argc, char** argv)
{
	ServerHelper m_server;
	m_server.Initialize();
	while (!m_server.g_doQuit)
	{
		m_server.GameLoop();
	}
	m_server.ShutDown();
	return 0;
}