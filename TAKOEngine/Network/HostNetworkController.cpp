#include <iostream>

#include "TAKOEngine/Network/NetworkController.h"
#include "TAKOEngine/Editor/Input/InputMap.h"

#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/Player.h"

#include "GameData.h"

bool HostNetworkController::Initialize(const char* address, const char* port)
{
	// TCPサーバー起動
	if (!tcpServerSocket.Connect(address, port))
	{
		return false;
	}
	// UDPサーバー起動
	if (!udpServerSocket.Connect(address, port))
	{
		return false;
	}

	return ClientNetworkController::Initialize(address, port);
}
void HostNetworkController::Finalize()
{
	tcpServerSocket.Disconnect();
	udpServerSocket.Disconnect();

	ClientNetworkController::Finalize();
}
void HostNetworkController::SendDisconnection()
{
	// TCP送信
	int sendData = TCP_COMMAND::COMMAND_DISCONNECT_CONFIRM;
	for (std::pair<const int, CLIENT_DATA>& client : clients)
	{
		tcpServerSocket.Send(client.first, &sendData, sizeof(sendData));
	}
}

void HostNetworkController::UpdateConnection()
{
	if (!tcpServerSocket.IsConnecting()) return;

	bool tcpSendFlag = false;

	// 接続中断したクライアント処理
	for (int id : disconnect_clients)
	{
		clients.erase(id);
	}
	disconnect_clients.clear();

	clientCount = static_cast<int>(clients.size());

	// 接続許可
	if (gameState == GAME_STATE::WAITING && clients.size() < MAX_CONNECTION)
	{
		if (tcpServerSocket.Accept())
		{
			int id = tcpServerSocket.GetClientCount() - 1;
			clients[id] = {}; // 新しいクライアント
			clients[id].conn = {
				id,
				GAME_STATE::WAITING,
				PLAYER_CLASS::KNIGHT
			};
			tcpSendFlag = true;
		}
	}
	// TCP受信 クライアント状態更新
	TCP_CONNECTION_DATA recData;
	for (std::pair<const int, CLIENT_DATA>& client : clients)
	{
		if (tcpServerSocket.Receive(client.first, &recData, sizeof(recData)) > 0) // クライアントから受信
		{
			std::cout << "TCP Receive DATA client_id: " << client.first << " classType:  " << recData.classType << " gameState: " << recData.gameState << std::endl;
			client.second.conn.classType = recData.classType;
			client.second.conn.colorIdx = recData.colorIdx;
			client.second.conn.gameState = recData.gameState;
			strcpy_s(client.second.conn.name, 64, recData.name);

			tcpSendFlag = true;
		}
	}

	tcpSendTimer -= TentacleLib::timer.Delta(); // 強制送信
	if (tcpSendTimer < 0.0f)
	{
		tcpSendTimer = tcpSendTime;
		tcpSendFlag = true;
	}

	if (tcpSendFlag || startFlag)
	{
		// TCP送信データ
		TCP_SERVER_DATA sendData = {};
		sendData.clientCount = clientCount;
		sendData.gameState = GAME_STATE::READY;
		int i = 0;
		for (std::pair<const int, CLIENT_DATA>& client : clients)
		{
			sendData.clients[i] = client.second.conn;
			if (client.second.conn.gameState == GAME_STATE::WAITING) sendData.gameState = GAME_STATE::WAITING;
			i++;
		}
		if (startFlag) sendData.gameState = GAME_STATE::GAME;

		// TCP送信
		for (std::pair<const int, CLIENT_DATA>& client : clients)
		{
			sendData.client_id = client.first;
			if (tcpServerSocket.Send(client.first, &sendData, sizeof(sendData)) < 0)
			{
				// 送信失敗、クライアント削除
				disconnect_clients.insert(client.first);
			}
		}
	}
	ClientNetworkController::UpdateConnection();
}

void HostNetworkController::Update()
{
	if (!udpServerSocket.IsConnecting()) return;

	// 接続確認
	CheckAlive();

	// UDP受信
	RecvUDP();

	// UDP送信
	SendUDP();

	ClientNetworkController::Update();
}

void HostNetworkController::CheckAlive()
{
	bool tcpSendFlag = false;

	// 接続中断したクライアント処理
	for (int id : disconnect_clients)
	{
		clients[id].player.playerData.connected = false;
		udpServerSocket.Remove(id);
	}
	disconnect_clients.clear();

	// TCP受信 クライアント状態更新
	int recData;	 // COMMAND 受信
	for (std::pair<const int, CLIENT_DATA>& client : clients)
	{
		if (tcpServerSocket.Receive(client.first, &recData, sizeof(recData)) > 0) // クライアントから受信
		{
			switch (recData)
			{
			case TCP_COMMAND::COMMAND_CONFIRM: break; // 生存確認用・処理なし
			case TCP_COMMAND::COMMAND_DISCONNECT_REQUEST: // 接続中断のリクエスト
				int command = TCP_COMMAND::COMMAND_DISCONNECT_CONFIRM;
				tcpServerSocket.Send(client.first, &command, sizeof(int)); // 中断・返事
				disconnect_clients.insert(client.first);
				break;
			}
		}
	}

	tcpSendTimer -= TentacleLib::timer.Delta(); // 強制送信
	if (tcpSendTimer < 0.0f)
	{
		tcpSendTimer = tcpSendTime;
		tcpSendFlag = true;
	}

	if (tcpSendFlag)
	{
		// TCP送信
		int sendData = COMMAND_CONFIRM;
		for (std::pair<const int, CLIENT_DATA>& client : clients)
		{
			if (tcpServerSocket.Send(client.first, &sendData, sizeof(sendData)) < 0)
			{
				// 送信失敗、クライアント削除
				disconnect_clients.insert(client.first);
			}
		}
	}
}

// UDP受信
void HostNetworkController::RecvUDP()
{
	struct sockaddr_in addr;
	UDP_CONNECTION_DATA recvData;
	while (udpServerSocket.Receive(&recvData, sizeof(recvData), addr) > 0)
	{
		std::cout << "UDP Receive DATA client_id: " << recvData.playerData.client_id << " Count:  " << recvData.count << std::endl;
		udpServerSocket.Register(recvData.playerData.client_id, addr);

		if (clients[recvData.playerData.client_id].player.count >= recvData.count) continue; // 過去・処理済みデータ
		CLIENT_DATA& client = clients[recvData.playerData.client_id];

		// 衝突判定結果受信
		for (int i = 0; i < MAX_COLLISION_RESULT; i++)
		{
			if (recvData.collisions[i].count < 0) break; // End Flag

			// 衝突
			CollsionReceive(recvData.playerData.client_id, recvData.collisions[i]);
		}

		client.player = recvData; // 新しいデータ保存
	}
}
// UDP送信
void HostNetworkController::SendUDP()
{
	// データ
	UDP_SERVER_DATA sendData;
	sendData.count = sendCount++;
	sendData.gameState = gameState;
	int i = 0;
	for (std::pair<const int, CLIENT_DATA>& client : clients)
	{
		sendData.playerData[i] = client.second.player.playerData;

		for (int j = 0; j < MAX_COLLISION_RESULT; j++)
		{
			sendData.collisions[i][j] = client.second.player.collisions[j];
		}
		i++;
	}
	int j = 0;
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		enemy->ExportData(sendData.enemyData[j]);
		j++;
	}

	udpServerSocket.SendAll(&sendData, sizeof(sendData));
}

void HostNetworkController::Reset()
{
	clients.clear();
	sendCount = 0;
	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		playersData[i].client_id = -1;
	}
	ClientNetworkController::Reset();
}

void HostNetworkController::GameStart()
{
	startFlag = true;;
}

void HostNetworkController::CollsionReceive(int client_id, ENEMY_COLLISION collision)
{
	CLIENT_DATA& client = clients[client_id];

	if (collision.count < 0) return;

	// 受信済みチェック
	for (int i = 0; i < MAX_COLLISION_RESULT; i++)
	{
		if (client.player.collisions[i].count == collision.count) return;
	}

	// 新しい結果
	Enemy* enemy = ENEMIES.GetEnemyById(collision.enemy_id);
	if (enemy)
	{
		enemy->OnDamage(collision);
	}
}