#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

enum InputFlags
{
	INPUT_W = 1,
	INPUT_A = 2,
	INPUT_S = 4,
	INPUT_D = 8,
	INPUT_FIRE = 16,
	INPUT_R = 32
};

DEFINE_ENUM_FLAG_OPERATORS(InputFlags);

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

enum ClientState {
	NotInitialized,
	Initialized,
	UnknownError,
};
class ClientHelper
{
public:
	ClientHelper();
	~ClientHelper();

	void Initialize();
	void Destroy();

	void SetServerAddressAndPort(std::string address, int port);

	bool SendUpdateToServer(UserInput& data);
	bool CheckForUpdateFromGameServer(GameScene& gameState);

	void Update(float deltaTime);
private:
	SOCKET m_ServerSocket;
	sockaddr_in m_AddrInfo;
	int m_AddressSize;
	ClientState m_NetworkState;
	float m_NextSendTime;
	float m_NextRecvTime;
	float m_SendTimeDelta;
	float m_RecvTimeDelta;
	float m_CurrentTime;
};