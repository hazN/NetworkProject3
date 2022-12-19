
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ClientHelper.h"

#include <iostream>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")

ClientHelper::ClientHelper()
	: m_ServerSocket(INVALID_SOCKET)
	, m_AddrInfo()
	, m_AddressSize(0)
	, m_NetworkState(NotInitialized)
	, m_SendTimeDelta(0.01f)
	, m_RecvTimeDelta(1 / 5.0f)
	, m_NextSendTime(0)
	, m_NextRecvTime(0)
	, m_CurrentTime(0)
{

}

ClientHelper::~ClientHelper()
{

}

void ClientHelper::Initialize()
{
	if (m_NetworkState == Initialized)
		return;

	// Initialization 
	WSADATA wsaData;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("NetworkClient::Initialize: WSAStartup failed with error %d\n", result);
		m_NetworkState = UnknownError;
		return;
	}

	// Create our UDP Socket
	m_ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_ServerSocket == INVALID_SOCKET) {
		printf("NetworkClient::Initialize: socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		m_NetworkState = UnknownError;
		return;
	}

	// Immediately set the socket to be Non-Blocking
	DWORD NonBlock = 1;
	result = ioctlsocket(m_ServerSocket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		printf("NetworkClient::Initialize: ioctlsocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		m_NetworkState = UnknownError;
		return;
	}

	m_NetworkState = Initialized;
}

void ClientHelper::Destroy()
{
	if (m_NetworkState != Initialized)
		return;

	closesocket(m_ServerSocket);
	WSACleanup();
}

void ClientHelper::SetServerAddressAndPort(std::string address, int port)
{
	if (m_NetworkState != Initialized)
		return;

	m_AddrInfo.sin_family = AF_INET;
	m_AddrInfo.sin_port = htons(port);
	m_AddrInfo.sin_addr.s_addr = inet_addr(address.c_str());

	m_AddressSize = sizeof(m_AddrInfo);
}

unsigned int g_MessageSendCount = 0;
bool ClientHelper::SendUpdateToServer(UserInput& data)
{
	if (m_NetworkState != Initialized)
		return false;

	if (m_CurrentTime < m_NextSendTime)
		return false;
	m_NextSendTime += m_SendTimeDelta;

	//printf("%.2f | %.2f > Sending input to the server!\n", m_CurrentTime, m_NextSendTime);

	data.id = g_MessageSendCount++;
	int sizeOfMessage = sizeof(data);	// This will be 8 bytes,
										// sizeof(bool) * 4 + messageId(int)
	/*
	// Example of the conversion:
	UserInputMessage* ptrToData = &data;

	// We need to *explicitly* cast the pointer to the 'const char*' type.
	const char* ptrToDataAsCharArray = (const char*)ptrToData;
	*/
	std::cout << "trying to send message, " << data.input << std::endl;
	int sendResult = sendto(
		m_ServerSocket,			// the SOCKET we created for our server
		(const char*)&data,		// The data we are sending
								// This is a POINTER to a UserInputMessage type
								// casted to a const char pointer.
		sizeOfMessage,			// The length of the message we are sending in bytes
		0,						// We are not setting any flags
		(SOCKADDR*)&m_AddrInfo, // The address of our server set in SetServerAddressAndPort
		m_AddressSize			// The length of our AddrInfo
	);

	if (sendResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("NetworkClient::SendUpdateToServer: send failed with error: %d\n", WSAGetLastError());
			closesocket(m_ServerSocket);
			WSACleanup();
			m_NetworkState = UnknownError;
			return false;
		}

		return false;
	}
	else if (sendResult > 0) {
		data.input = 0;
		return true;
	}
	return false;
}

bool ClientHelper::CheckForUpdateFromGameServer(GameScene& gameState)
{
	if (m_NetworkState != Initialized)
		return false;

	//if (m_CurrentTime < m_NextRecvTime)
	//	return gameState;
	//m_NextRecvTime += m_RecvTimeDelta;

	const int BUF_SIZE = 512;	// Size of the buffer, that is the maximum we can recv in this call
	char buf[512];

	int recvResult = recvfrom(m_ServerSocket, buf, BUF_SIZE, 0, (SOCKADDR*)&m_AddrInfo, &m_AddressSize);
	if (recvResult == SOCKET_ERROR)
	{
		int lastError = WSAGetLastError();
		if (lastError != WSAEWOULDBLOCK)
		{
			/*
			* The virtual circuit was reset by the remote side executing a hard or abortive close.
			* The application should close the socket; it is no longer usable. On a UDP-datagram
			* socket this error indicates a previous send operation resulted in an ICMP Port
			* Unreachable message.
			*/
			if (lastError == WSAECONNRESET) {
				printf("NetworkClient::CheckForUpdateFromGameServer: recv failed, the server was unreachable in the last send call.");
				m_NetworkState = UnknownError;
				return false;
			}

			printf("NetworkClient::CheckForUpdateFromGameServer: recv failed with error: %d\n", WSAGetLastError());
			closesocket(m_ServerSocket);
			WSACleanup();
			m_NetworkState = UnknownError;
		}

		return false;
	}

	memcpy(&gameState, (const void*)buf, sizeof(GameScene));

	return true;
}

void ClientHelper::Update(float deltaTime)
{
	if (deltaTime > 100)
		deltaTime = 0;
	m_CurrentTime += deltaTime;
}
