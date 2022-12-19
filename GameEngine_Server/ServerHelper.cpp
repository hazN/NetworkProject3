#include "ServerHelper.h"

#include <cassert>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "gameprotocol.pb.h"
int g_Iteration;
GameScene g_GameScene;
ClientInfo g_RecvClientInfo;

std::vector<ClientInfo> g_ClientInfo;
std::vector<UserInput> g_PlayerMessages;

std::chrono::steady_clock::time_point g_LastUpdateTime;
std::chrono::steady_clock::time_point g_CurrentUpdateTime;
float g_CurrentTime = 0;
float g_NextNetworkSend = 0;
float g_SendDeltaTime = 0.1f;
float g_NextNetworkRecv = 0;
float g_RecvDeltaTime = 1 / 5.f;
float deltaTimeInSeconds;
unsigned int g_LatestMessageId;

//Method : Shutdown
//Summary: Shuts down the server
//Params : void
//Returns: void
void ServerHelper::ShutDown()
{
	// Close
	printf("Closing . . . \n");
	closesocket(listenSocket);
	WSACleanup();
}

//Method : Initialize
//Summary: Starts the server
//Params : void
//Returns: void
int ServerHelper::Initialize()
{
	// WSA SETUP
	WSADATA wsaData;
	int result;

	printf("WSAStartup . . . ");
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	else {
		printf("Success!\n");
	}

	// CREATE SOCKET
	printf("Creating our Listen Socket . . . ");
	listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (listenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("socket: Success!\n");
	}

	DWORD NonBlock = 1;
	result = ioctlsocket(listenSocket, FIONBIO, &NonBlock);
	if (result != 0) {
		wprintf(L"ioctlsocket failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("ioctlsocket: Success!\n");
	}

	struct sockaddr_in recvAddr;
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(5555);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(listenSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
	if (result != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else {
		printf("bind: Success!\n");
	}

	g_RecvClientInfo.clientAddrSize = sizeof(g_RecvClientInfo.clientAddr);
	g_RecvClientInfo.HaveInfo = false;
	direction[0] = UP;
	direction[1] = UP;
	direction[2] = UP;
	direction[3] = UP;
	return 0;
}

//Method : FindPlayerByPort
//Summary: Find client using their address 
//Params : int, ClientInfo
//Returns: bool
bool ServerHelper::FindPlayerByPort(int port, ClientInfo& clientInfo)
{
	for (int i = 0; i < g_ClientInfo.size(); i++) {
		if (g_ClientInfo[i].clientAddr.sin_port == port) {
			clientInfo = g_ClientInfo[i];
			return true;
		}
	}
	return false;
}

//Method : RecvFromClient
//Summary: Get the input from the client
//Params : void
//Returns: int
int ServerHelper::RecvFromClient()
{
	const int bufsize = sizeof(UserInput);
	char buf[bufsize];

	// RECEIVE CALL
	int recvResult = recvfrom(
		listenSocket,
		buf,
		bufsize,
		0,
		(SOCKADDR*)&g_RecvClientInfo.clientAddr,
		&g_RecvClientInfo.clientAddrSize
	);

	// ERROR CHECK IT
	if (recvResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			g_doQuit = true;
		}
		return 0;
	}

	if (recvResult == 0)
	{
		int breakpoint = 0;
		return 0;
	}
	ClientInfo clientInfo;

	USHORT port = g_RecvClientInfo.clientAddr.sin_port;
	if (FindPlayerByPort(port, clientInfo) == false) {
		USHORT numPlayersConnected = g_ClientInfo.size();

		// Add new player
		clientInfo.clientAddr = g_RecvClientInfo.clientAddr;
		clientInfo.clientAddrSize = g_RecvClientInfo.clientAddrSize;
		clientInfo.HaveInfo = true;
		clientInfo.playerId = numPlayersConnected;
		g_ClientInfo.push_back(clientInfo);
		g_PlayerMessages.push_back(UserInput());
		Sphere* bullet = new Sphere(glm::vec3(0), 0.5f);
		g_GameScene.players[numPlayersConnected].state = IS_CONNECTED | IS_ACTIVE | HAS_AMMO;
		g_bullets[numPlayersConnected] = physics_system.CreatePhysicsObject(glm::vec3(0), bullet, "Bullet" + numPlayersConnected);
	}

	UserInput& userInputMessage = g_PlayerMessages[clientInfo.playerId];

	memcpy(&userInputMessage, (const void*)buf, bufsize);

	int hasUserInput = userInputMessage.input;
	std::cout << "trying to receive" << userInputMessage.input << std::endl;

	if (hasUserInput != 0)
	{
		GameUpdate();
	}

	return 1;
}

//Method : GetInfoFromClients
//Summary: Loop the RecvFromClient
//Params : void
//Returns: void
void ServerHelper::GetInfoFromClients()
{

	int RecvCount = 0;
	bool stopReading = false;
	while (!stopReading)
	{
		int recvResult = RecvFromClient();
		RecvCount += recvResult;

		if (recvResult == 0)
			stopReading = true;
	}
	//if (RecvCount > 0) {
	//	printf("  -----  -----  -----  \n");
	//}
}

//Method : UpdateClientsWithGameState
//Summary: Send the gamestate back to the players
//Params : void
//Returns: void
void ServerHelper::UpdateClientsWithGameState()
{

	if (g_CurrentTime < g_NextNetworkSend)
		return;
	g_NextNetworkSend = g_CurrentTime + g_SendDeltaTime;

	//gameprotocol::GameScene scene;
	//scene.set_id(g_GameScene.id);
	//for (int i = 0; i < 4; i++)
	//{
	//	gameprotocol::Player* player = scene.add_players();
	//	gameprotocol::vector3 playerposition;
	//	player->set_state(g_GameScene.players[i].state);
	//	playerposition.set_x(g_GameScene.players[i].position[0]);
	//	playerposition.set_y(g_GameScene.players[i].position[1]);
	//	playerposition.set_z(g_GameScene.players[i].position[2]);
	//	player->set_allocated_position(&playerposition);
	//	gameprotocol::quaternion orientation;
	//	orientation.set_w(g_GameScene.players[i].quaternion[0]);
	//	orientation.set_x(g_GameScene.players[i].quaternion[1]);
	//	orientation.set_y(g_GameScene.players[i].quaternion[2]);
	//	orientation.set_z(g_GameScene.players[i].quaternion[3]);
	//	player->set_allocated_orientation(&orientation);
	//	gameprotocol::Bullet* bullet = scene.add_bullets();
	//	gameprotocol::vector3 bulletposition;
	//	bulletposition.set_x(g_GameScene.players[i].position[0]);
	//	bulletposition.set_y(g_GameScene.players[i].position[1]);
	//	bulletposition.set_z(g_GameScene.players[i].position[2]);
	//}
	//std::string serializedString;
	//scene.SerializeToString(&serializedString);
	int gameStateSize = sizeof(g_GameScene);
	g_GameScene.id = g_LatestMessageId;

	for (int i = 0; i < g_ClientInfo.size(); i++) {
		const ClientInfo& clientInfo = g_ClientInfo[i];
		int sendResult = sendto(
			listenSocket,
			(const char*)&g_GameScene,
			gameStateSize,
			0,
			(SOCKADDR*)&clientInfo.clientAddr,
			clientInfo.clientAddrSize
		);

		if (sendResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf("[%d] send failed with error: %d\n", i, WSAGetLastError());
				closesocket(listenSocket);
				WSACleanup();
				g_doQuit = true;
			}

			return;
		}
	}
}
//Method : GetPlayerInfoReferenceById
//Summary: Get player by id
//Params : USHORT
//Returns: PlayerInfo
PlayerInfo& ServerHelper::GetPlayerInfoReferenceById(USHORT id)
{
	if (id == 0) return g_GameScene.players[0];
	if (id == 1) return g_GameScene.players[1];
	if (id == 2) return g_GameScene.players[2];
	if (id == 3) return g_GameScene.players[3];

	assert(-1);
	return g_GameScene.players[0];
}
//Method : GetBulletInfoReferenceById
//Summary: Get bullet by id
//Params : USHORT
//Returns: BulletInfo
BulletInfo& ServerHelper::GetBulletInfoReferenceById(USHORT id)
{
	if (id == 0) return g_GameScene.bullets[0];
	if (id == 1) return g_GameScene.bullets[1];
	if (id == 2) return g_GameScene.bullets[2];
	if (id == 3) return g_GameScene.bullets[3];

	assert(-1);
	return g_GameScene.bullets[0];
}
//Method : UpdatePlayerObjects
//Summary: Get the userinputs and update the physics objects 
//Params : void
//Returns: void
void ServerHelper::UpdatePlayerObjects() {
	for (int i = 0; i < g_PlayerMessages.size(); i++) {
		UserInput& playerMessage = g_PlayerMessages[i];
		PlayerInfo& playerInfo = GetPlayerInfoReferenceById(i);
		// Update the game state
		if (playerInfo.state & IS_ACTIVE)
		{
			if (playerMessage.input & INPUT_W) {
				switch (direction[i])
				{
				case UP:
					playerInfo.position[2] += 5.f;
					break;
				case DOWN:
					playerInfo.position[2] -= 5.f;
					break;
				case LEFT:
					playerInfo.position[0] += 5.f;
					break;
				case RIGHT:
					playerInfo.position[0] -= 5.f;
					break;
				default:
					playerInfo.position[2] += 5.f;
					break;
				}
			}
			if (playerMessage.input & INPUT_S) {
				switch (direction[i])
				{
				case UP:
					playerInfo.position[2] -= 5.f;
					break;
				case DOWN:
					playerInfo.position[2] += 5.f;
					break;
				case LEFT:
					playerInfo.position[0] -= 5.f;
					break;
				case RIGHT:
					playerInfo.position[0] += 5.f;
					break;
				default:
					playerInfo.position[2] -= 5.f;
					break;
				}
			}
			if (playerMessage.input & INPUT_A) {
				glm::quat playerRotation;
				switch (direction[i])
				{
				case UP:
					direction[i] = LEFT;
					playerRotation.y = 90.f;
					break;
				case DOWN:
					direction[i] = RIGHT;
					playerRotation.y = -90.f;
					break;
				case LEFT:
					direction[i] = DOWN;
					playerRotation.y = -180.f;
					break;
				case RIGHT:
					direction[i] = UP;
					playerRotation.y = 0.f;
					break;
				default:
					direction[i] = LEFT;
					playerRotation.y = 90.f;
					break;
				}
				g_GameScene.players[i].quaternion[0] = 1.f;
				g_GameScene.players[i].quaternion[2] = playerRotation.y;
			}
			if (playerMessage.input & INPUT_D) {
				glm::quat playerRotation;
				switch (direction[i])
				{
				case UP:
					direction[i] = RIGHT;
					playerRotation.y = -90.f;
					break;
				case DOWN:
					direction[i] = LEFT;
					playerRotation.y = 90.f;
					break;
				case LEFT:
					direction[i] = UP;
					playerRotation.y = 0.f;
					break;
				case RIGHT:
					direction[i] = DOWN;
					playerRotation.y = -180.f;
					break;
				default:
					direction[i] = RIGHT;
					playerRotation.y = -90.f;
					break;
				}
				g_GameScene.players[i].quaternion[0] = 1.f;
				g_GameScene.players[i].quaternion[2] = playerRotation.y;
			}
			if (playerMessage.input & INPUT_FIRE) {
				if (playerInfo.state & HAS_AMMO)
				{
					playerInfo.state &= ~HAS_AMMO;
					g_bullets[i]->position = glm::vec3(g_GameScene.players[i].position[0], g_GameScene.players[i].position[1], g_GameScene.players[i].position[2]);
					switch (direction[i])
					{
					case UP:
						direction[i] = RIGHT;
						g_bullets[i]->ApplyForce(glm::vec3(0.f, 0.f, 20.01f));
						break;
					case DOWN:
						direction[i] = LEFT;
						g_bullets[i]->ApplyForce(glm::vec3(0.f, 0.f, -20.01f));
						break;
					case LEFT:
						direction[i] = UP;
						g_bullets[i]->ApplyForce(glm::vec3(20.01f, 0.f, 0.f));
						break;
					case RIGHT:
						direction[i] = DOWN;
						g_bullets[i]->ApplyForce(glm::vec3(-20.01f, 0.f, 0.f));
						break;
					default:
						direction[i] = RIGHT;
						g_bullets[i]->ApplyForce(glm::vec3(0.f, 0.f, 20.01f));
						break;
					}
					g_bullets[i]->deltaTime = std::clock();
				}
			}
		}
		else
		{
			if (playerMessage.input & INPUT_R) {
				playerInfo.state = IS_ACTIVE | IS_CONNECTED | HAS_AMMO;
			}
		}
		if (!(playerInfo.state & HAS_AMMO))
		{
			g_bullets[i]->duration = (std::clock() - g_bullets[i]->deltaTime) / (double)CLOCKS_PER_SEC;
			if(g_bullets[i]->duration > 2)
			{
				g_bullets[i]->KillAllForces();
				playerInfo.state |= HAS_AMMO;
				g_GameScene.bullets[i].position[1] = -50.f;
				break;
			}
			g_bullets[i]->Integrate(0.0001f);
			g_GameScene.bullets[i].position[0] = g_bullets[i]->position.x;
			g_GameScene.bullets[i].position[1] = g_bullets[i]->position.y;
			g_GameScene.bullets[i].position[2] = g_bullets[i]->position.z;
			for (int i2 = 0; i2 < 4; i2++)
			{
				if (i2 != i)
				{
					glm::vec3 playerPos = glm::vec3(g_GameScene.players[i2].position[0], g_GameScene.players[i2].position[1], g_GameScene.players[i2].position[2]);
					glm::vec3 bulletPos = glm::vec3(g_bullets[i]->position[0], g_bullets[i]->position[1], g_bullets[i]->position[2]);
					if (glm::length(playerPos - bulletPos) < 1.f)
					{
						g_GameScene.players[i2].state &= ~IS_ACTIVE;
						g_bullets[i]->KillAllForces();
						playerInfo.state |= HAS_AMMO;
						g_GameScene.bullets[i].position[1] = -50.f;
					}
				}
			}
		}
		playerMessage.input = 0;
	}
}

void ServerHelper::GameUpdate()
{
	UpdatePlayerObjects();
}

//Method : GameLoop
//Summary: Main server-game loop
//Params : void
//Returns: void
void ServerHelper::GameLoop()
{
	g_LastUpdateTime = g_CurrentUpdateTime;
	g_CurrentUpdateTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = g_CurrentUpdateTime - g_LastUpdateTime;
	deltaTimeInSeconds = elapsed.count();
	if (deltaTimeInSeconds > 1) {
		return;
	}

	if (deltaTimeInSeconds < 100)
		g_CurrentTime += deltaTimeInSeconds;

	GetInfoFromClients();
	
	GameUpdate();

	UpdateClientsWithGameState();
}