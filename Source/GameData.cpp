#include "GameData.h"

#include <fstream>

#include "TAKOEngine/Network/url.h"

GameData::GameData()
{
	if (!LoadGameSetting()) // ロード失敗・新しいファイル
	{
		SaveGameSetting();
	}
}

bool GameData::SaveGameSetting()
{
	std::ofstream ofs;
	ofs.open("setting.bin", std::ios::binary);
	if (!ofs)	// ファイル失敗
	{
		return false;
	}
	ofs.write((const char*)&gameSetting, sizeof(GameData::GAME_SETTING));
	ofs.close();

	return true;
}
bool GameData::LoadGameSetting()
{
	std::ifstream ifs;
	ifs.open("setting.bin", std::ios::binary);
	if (!ifs) { // オープン失敗時
		gameSetting = {}; // デフォルトデータ
		return false;
	}

	ifs.read((char*)&gameSetting, sizeof(GameData::GAME_SETTING));
	ifs.close();

	return true;
}

void GameData::Logout()
{
	userData = {};
	memset(GAME_DATA.GetSetting().token, 0, sizeof(GAME_DATA.GetSetting().token));
	SaveGameSetting();
}

void GameData::CheckServerExit()
{
}