/*
	Hassan Assaf
	INFO-6016
	Project #3: Multiplayer Game
	Due 2022-12-18
*/
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <iostream>
//#include "Buffer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "PhysicsSystem.h"

// Linking Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// Setting up port
#define DEFAULT_PORT "5555"

struct BulletInfo;
struct PlayerInfo;
struct ClientInfo;

class ServerHelper
{
public:
	void ShutDown();
	int Initialize();

	bool g_doQuit = false;
	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET connectSocket = INVALID_SOCKET;
	fd_set activeSockets;
	fd_set socketsReadyForReading;
	std::vector<ClientInfo> clients;
	bool FindPlayerByPort(int port, ClientInfo& clientInfo);
	int RecvFromClient();
	void GetInfoFromClients();
	void UpdateClientsWithGameState();
	PlayerInfo& GetPlayerInfoReferenceById(USHORT id);
	BulletInfo& GetBulletInfoReferenceById(USHORT id);
	void UpdatePlayerObjects();
	void GameUpdate();
	void GameLoop();
	PhysicsSystem physics_system;
	PhysicsObject* g_bullets[4];
	int direction[4];
};
enum Directions
{
	UP, DOWN, RIGHT, LEFT,
};
enum InputFlags
{
	INPUT_W = 1,
	INPUT_A = 2,
	INPUT_S = 4,
	INPUT_D = 8,
	INPUT_FIRE = 16,
	INPUT_R = 32
};

DEFINE_ENUM_FLAG_OPERATORS(InputFlags)

enum StateFlags
{
	IS_ACTIVE = 1,
	IS_CONNECTED = 2,
	HAS_AMMO = 4,
};
DEFINE_ENUM_FLAG_OPERATORS(StateFlags)
// Make it easier to choose msg id
enum MsgType { JOIN = 1, LEAVE = 2, SEND = 3, CREATE = 4, AUTHENTICATE = 5 };

struct ClientInfo {
	bool HaveInfo;
	sockaddr_in clientAddr;
	int clientAddrSize;
	USHORT playerId;
};

struct PlayerInfo {
	SOCKET socket;
	int state;
	float position[3];
	float velocity[3];
	float quaternion[4];
};

struct BulletInfo {
	int state;
	float position[3];
	float velocity[3];
};

struct GameScene {
	int id;
	PlayerInfo players[4];
	BulletInfo bullets[4];
};
struct PacketHeader
{
	int length;
	int id;
};

struct UserInput
{
	int id;
	int input;
};

struct GamePacket
{
	PacketHeader header;
	std::string serializedString;
};