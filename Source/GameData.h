#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Editor/Camera/CameraController.h"

#include <string>

class GameData : public Singleton<GameData>
{
	friend class Singleton <GameData>;
private:
	GameData();
	~GameData() = default;
public:
	void SetClientId(int id) { client_id = id; }
	int GetClientId() { return client_id; }
	const char* GetName() { return name.c_str(); }

	void SetOnline(bool online) { this->online = online; }
	bool IsOnline() { return this->online; }

	void SetHost(bool host) { isHost = host; }
	void SetName(const char* name) { this->name = name; }
	bool IsHost() { return isHost; }

	bool SeverExist() { return serverExisted; }
	void CheckServerExit();
public:
	struct GameSetting
	{
		float CameraSensitivity = 5.0f;		// カメラ感度
		bool CamerainvertX = false;			// カメラX反転
		bool CamerainvertY = false;			// カメラY反転
		bool KeyboardInput = true;			// キーボード / ゲームパッド

		char token[64];					// ログイントークン
	};
	struct UserData							// ログイン
	{
		int id = -1;
		std::string useranme = "";
		std::string nickname = "";
		int gems = 0;
	};

	GameSetting& GetSetting() { return gameSetting; }
	UserData& GetUser() { return userData; }
	void UpdateSetting(const GameSetting& setting) { gameSetting = setting; };
	void UpdateUser(const UserData& user) { userData = user; };
	void Logout();
	bool SaveGameSetting();	// 設定ファイルの書き出し(保存)
	std::string& GetIp() { return ip; }
	void SetIp(std::string ip) { this->ip = ip; }
	void SetIp(const char* ip) { this->ip = ip; }
private:
	bool LoadGameSetting();	// 設定ファイルの読み込み
private:
	bool online = false;
	int client_id = -1;
	bool isHost = false;
	std::string name;
	std::string ip;

	bool serverExisted = false;

	GameSetting gameSetting = {};
	UserData userData = {};
};

#define GAME_DATA GameData::Instance()
#define GAME_SETTING GameData::Instance().GetSetting()