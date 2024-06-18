#pragma once

#include <map>
#include <set>

#include "TAKOEngine/Network/TCPSocket.h"
#include "TAKOEngine/Network/UDPSocket.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "GameObject/Character/Player/PlayerManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

#define MAX_CONNECTION 5			// 最大接続数
#define MAX_ENEMIES 20				// 最大敵数
#define MAX_COLLISION_RESULT 5		// 衝突判定結果 (プレイヤー → 敵)

enum GAME_STATE
{
	WAITING = 0,
	READY,
	GAME,
	GAME_OVER,
	WIN,
};

enum TCP_COMMAND
{
	COMMAND_CONFIRM,
	COMMAND_DISCONNECT_REQUEST,
	COMMAND_DISCONNECT_CONFIRM,
};

struct TCP_CONNECTION_DATA
{
	int client_id = -1;
	int gameState = -1;
	int classType = -1;
	int colorIdx = -1;
	char name[64];
};

struct TCP_SERVER_DATA
{
	int client_id = -1;
	int clientCount = 0;
	int gameState = -1;
	TCP_CONNECTION_DATA clients[MAX_CONNECTION];
};

struct UDP_CONNECTION_DATA
{
	int count = -1;
	PLAYER_DATA playerData;
	ENEMY_COLLISION collisions[MAX_COLLISION_RESULT];
};

struct UDP_SERVER_DATA
{
	int count = -1;
	int gameState = -1;
	PLAYER_DATA playerData[MAX_CONNECTION];
	ENEMY_DATA enemyData[MAX_ENEMIES];
	ENEMY_COLLISION collisions[MAX_CONNECTION][MAX_COLLISION_RESULT];
};

struct CLIENT_DATA
{
	TCP_CONNECTION_DATA conn;
	UDP_CONNECTION_DATA player;
};

class NetworkController
{
public:
	NetworkController() = default;
	~NetworkController() = default;

	virtual bool Initialize(const char* address, const char* port) = 0;
	virtual void Finalize() = 0;
	virtual void SendDisconnection() = 0;

	virtual void UpdateConnection() = 0;
	virtual void Update() = 0;
	virtual void Reset() = 0;

	virtual void GameStart() {}

	void SetGameState(int gameState) { this->gameState = gameState; }
	int GetGameState() { return gameState; }
	int GetClientCount() { return clientCount; }
	int GetClassType() { return classType; }
	int GetColorIndex() { return colorIdx; }

	void SetClassType(int type) { classType = type; tcpSendFlag = true; }
	void SetColorIndex(int idx) { colorIdx = idx; tcpSendFlag = true; }

	bool IsHost() { return isHost; }
	void ReadyTrigger() { isReady = !isReady; tcpSendFlag = true; }
	bool IsReady() { return isReady; }
protected:
	int clientCount = 0;
	int gameState = GAME_STATE::WAITING;
	int classType = PLAYER_CLASS::KNIGHT;
	int colorIdx = Player::COLOR_PATTERN::DEFAULT;

	bool isHost = false;
	bool isReady = false;
	bool tcpSendFlag = true;

	float tcpSendTime = 5.0f; // 5秒ごとに送信
};

class ClientNetworkController : public NetworkController
{
public:
	ClientNetworkController() = default;
	~ClientNetworkController() = default;

	virtual bool Initialize(const char* address, const char* port) override;
	virtual void Finalize() override;
	void SendDisconnection() override;

	virtual void UpdateConnection() override;
	virtual void Update() override;

	virtual void Reset() override;
private:
	// ゲーム中の接続確認 TCP
	void CheckAlive();
	// UDP受信
	void RecvUDP();
	// UDP送信
	void SendUDP();

	void UpdatePlayers();
private:
	float tcpSendTimer = tcpSendTime;
	int recvCount = 0;
	int sendCount = 0;

	TCP_CONNECTION_DATA players[MAX_CONNECTION];

	TCPClientSocket tcpClientSocket;
	UDPClientSocket udpClientSocket;
};

class HostNetworkController : public ClientNetworkController
{
public:
	HostNetworkController() { isHost = true; };
	~HostNetworkController() = default;

	virtual bool Initialize(const char* address, const char* port) override;
	virtual void Finalize() override;
	void SendDisconnection() override;

	void GameStart() override;

	// 接続待ち・キャラ選択同期
	virtual void UpdateConnection() override;

	virtual void Update() override;

	void Reset() override;
private:
	// Update
	// ゲーム中の接続確認 TCP
	void CheckAlive();
	// UDP受信
	void RecvUDP();
	// UDP送信
	void SendUDP();
	// 衝突結果の処理
	void CollsionReceive(int client_id, ENEMY_COLLISION collision);

private:
	float tcpSendTimer = tcpSendTime;
	int sendCount = 0;
	PLAYER_DATA playersData[MAX_CONNECTION];

	bool startFlag = false;
	TCPServerSocket tcpServerSocket = TCPServerSocket(MAX_CONNECTION);
	UDPServerSocket udpServerSocket;
	std::map<int, CLIENT_DATA> clients;
	std::set<int> disconnect_clients;
};