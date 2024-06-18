#include <iostream>

#include "TAKOEngine/Network/NetworkController.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"

#include "GameObject/Character/Player/Knight.h"
#include "GameObject/Character/Player/Mage.h"
#include "GameObject/Character/Player/Barbarian.h"
#include "GameObject/Character/Player/Rogue.h"

#include "GameData.h"

Player* PlayerFactor(int playerClass)
{
	Player* player = nullptr;
	switch (playerClass)
	{
	case PLAYER_CLASS::KNIGHT:
		player = new Knight;
		break;
	case PLAYER_CLASS::MAGE:
		player = new Mage;
		break;

	case PLAYER_CLASS::BARBARIAN:
		player = new Barbarian;
		break;
	case PLAYER_CLASS::ROGUE:
		player = new Rogue;
		break;
	}
	return player;
}

bool ClientNetworkController::Initialize(const char* address, const char* port)
{
	// TCPサーバーへ接続
	if (!tcpClientSocket.Connect(address, port))
	{
		return false;
	}
	// UDP
	if (!udpClientSocket.Connect(address, port))
	{
		return false;
	}

	return true;
}
void ClientNetworkController::Finalize()
{
	tcpClientSocket.Disconnect();
	udpClientSocket.Disconnect();
}
void ClientNetworkController::SendDisconnection()
{
	// TCP送信
	if (!tcpClientSocket.IsConnecting()) return;
	int sendData = TCP_COMMAND::COMMAND_DISCONNECT_REQUEST;
	tcpClientSocket.Send(&sendData, sizeof(sendData));
}

void ClientNetworkController::UpdateConnection()
{
	if (!tcpClientSocket.IsConnecting()) return;

	// TCP受信
	TCP_SERVER_DATA recvData;
	if (tcpClientSocket.Receive(&recvData, sizeof(recvData)) > 0) // データ受信
	{
		// 全体データ
		GAME_DATA.SetClientId(recvData.client_id);
		gameState = recvData.gameState;
		clientCount = recvData.clientCount;

		// ClientData
		for (int i = 0; i < MAX_CONNECTION; i++)
		{
			players[i].client_id = recvData.clients[i].client_id;

			players[i].gameState = recvData.clients[i].gameState;
			if (recvData.clients[i].client_id == recvData.client_id)
			{
				// 自分
				players[i].classType = classType;
				players[i].colorIdx = colorIdx;
				strcpy_s(players[i].name, 64, GAME_DATA.GetName());
			}
			else
			{
				players[i].classType = recvData.clients[i].classType;
				players[i].colorIdx = recvData.clients[i].colorIdx;
				strcpy_s(players[i].name, 64, recvData.clients[i].name);
			}
		}
	}

	tcpSendTimer -= TentacleLib::timer.Delta(); // 強制送信
	if (tcpSendTimer < 0.0f)
	{
		tcpSendTimer = tcpSendTime;
		tcpSendFlag = true;
	}

	// TCP送信
	if (GAME_DATA.GetClientId() >= 0 && tcpSendFlag) // client
	{
		TCP_CONNECTION_DATA sendData = {
			GAME_DATA.GetClientId(),
			(isReady) ? GAME_STATE::READY : GAME_STATE::WAITING,
			classType,
			colorIdx,
		};
		strcpy_s(sendData.name, 64, GAME_DATA.GetName());
		if (tcpClientSocket.Send(&sendData, sizeof(sendData)) < 0)
		{
			// サーバー接続失敗
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
		}
		tcpSendFlag = false;
	}

	UpdatePlayers();
}
void ClientNetworkController::UpdatePlayers() // クライアントキャラ更新
{
	for (int i = 0; i < clientCount; i++)
	{
		if (players[i].client_id < 0) {
			continue;
		}
		Player* player = nullptr;
		if (PLAYERS.Count() < (i + 1)) // 存在しない
		{
			player = PLAYERS.Register(PlayerFactor(players[i].classType));
		}
		else {
			player = PLAYERS.get(i);
			if (player->GetClassType() != players[i].classType)
			{
				player = PLAYERS.Replace(i, PlayerFactor(players[i].classType));
			}
		}
		player->SetClientId(players[i].client_id);
		player->SetColor(Player::GetColorSet(players[i].colorIdx));
		player->SetName(players[i].name);

		// 準備完了ダンス
		if (players[i].gameState == GAME_STATE::READY && player->GetState() != static_cast<int>(Player::State::Ready))
		{
			player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Ready));
		}
		// 準備待機
		if (players[i].gameState == GAME_STATE::WAITING && player->GetState() != static_cast<int>(Player::State::Waiting))
		{
			player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Waiting));
		}
	}
	PLAYERS.Resize(clientCount);
}

void ClientNetworkController::Update()
{
	if (!udpClientSocket.IsConnecting()) return;

	CheckAlive();

	// UDP送信
	SendUDP();

	// UDP受信
	RecvUDP();
}
//
void ClientNetworkController::CheckAlive()
{
	bool tcpSendFlag = false;
	// TCP受信
	int recvData;
	if (tcpClientSocket.Receive(&recvData, sizeof(recvData)) > 0) // データ受信
	{
		switch (recvData)
		{
		case TCP_COMMAND::COMMAND_CONFIRM: break; // 生存確認用・処理なし
		case TCP_COMMAND::COMMAND_DISCONNECT_CONFIRM: // 接続中断のリクエストの返事
			// 中断確認
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			return;
		}
	}

	// TCP送信
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

		if (tcpClientSocket.Send(&sendData, sizeof(sendData)) < 0) // 確認
		{
			// 送信失敗、接続中断
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
			return;
		}
	}
}

// UDP受信
void ClientNetworkController::RecvUDP()
{
	UDP_SERVER_DATA recvData;
	if (udpClientSocket.Receive(&recvData, sizeof(recvData)) > 0)
	{
		if (recvData.count > recvCount)
		{
			gameState = recvData.gameState;
			std::cout << "Receive DATA from Server: " << recvData.count << std::endl;
			// 新しいデータ
			for (int i = 0; i < MAX_CONNECTION; i++) // プレイヤーデータ
			{
				PLAYER_DATA& data = recvData.playerData[i];
				if (data.client_id < 0) continue;

				if (data.client_id != GAME_DATA.GetClientId()) // 自分以外
				{
					Player* player = PLAYERS.GetPlayerById(data.client_id);
					if (player != nullptr)
					{
						if (data.connected)
						{
							PLAYERS.GetPlayerById(data.client_id)->ImportData(data); // データ更新
						}
						else
						{
							// 中断接続済み
							PLAYERS.Remove(player);
						}
					}
				}
				else // 自分
				{
					// 処理済み衝突結果を削除
					for (int j = 0; j < MAX_COLLISION_RESULT; j++)
					{
						if (recvData.collisions[i][j].count < 0) break;
						ENEMYCOLLISIONS.RemoveById(recvData.collisions[i][j].count);
					}
				}
			}

			for (int i = 0; i < MAX_ENEMIES; i++) // エネミーデーター
			{
				ENEMY_DATA& data = recvData.enemyData[i];
				if (data.enemy_id < 0) break; // エンドフラグ
				// エネミー更新
				if (!isHost)
				{
					ENEMIES.UpdateOrCreate(data);
				}
			}

			recvCount = recvData.count;
		}
	}
}

// UDP送信
void ClientNetworkController::SendUDP()
{
	UDP_CONNECTION_DATA sendData;
	sendData.count = sendCount++;
	PLAYERS.GetPlayerById(GAME_DATA.GetClientId())->ExportData(sendData.playerData);
	for (int i = 0; i < min(MAX_COLLISION_RESULT, ENEMYCOLLISIONS.Count()); i++)
	{
		int count = ENEMYCOLLISIONS.Count();

		ENEMY_COLLISION* collision = ENEMYCOLLISIONS.get(i);
		sendData.collisions[i] = *collision;
	}
	udpClientSocket.Send(&sendData, sizeof(sendData));
}

void ClientNetworkController::Reset()
{
	recvCount = 0;
	for (int i = 0; i < MAX_CONNECTION; i++)
	{
		players[i].client_id = -1;
		players[i].classType = -1;
		players[i].colorIdx = -1;
		players[i].gameState = -1;
	}
}