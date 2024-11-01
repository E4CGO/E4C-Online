#include "DungeonData.h"

DungeonData::DungeonData()
{
	// 部屋タイプの数で配列をリサイズする
	m_roomGenerateSettings.resize(ENUM_COUNT);

	// SIMPLE_ROOM_1
	{
		RoomGenerateSetting setting;
		setting.weight = 30;
		m_roomGenerateSettings.at(SIMPLE_ROOM_1) = setting;
	}

	// END_ROOM
	{
		RoomGenerateSetting setting;
		setting.weight = 0;
		m_roomGenerateSettings.at(END_ROOM) = setting;
	}

	// CROSS_ROOM_1
	{
		RoomGenerateSetting setting;
		setting.weight = 10;
		m_roomGenerateSettings.at(CROSS_ROOM_1) = setting;
	}

	// PASSAGE_1
	{
		RoomGenerateSetting setting;
		setting.weight = 60;
		m_roomGenerateSettings.at(PASSAGE_1) = setting;
	}



	// ダンジョンの生成設定
	m_dungeonGenerateSettings.maxDepth = 3;
}