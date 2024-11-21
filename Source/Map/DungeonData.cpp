#include "DungeonData.h"

DungeonData::DungeonData()
{
	// 部屋タイプの数で配列をリサイズする
	m_roomGenerateSettings.resize(ENUM_COUNT);

	// SIMPLE_ROOM_1
	{
		RoomGenerateSetting setting;
		setting.weight = 50;
		setting.aabb.position = { 0.0f, 3.0f, 8.0f };
		setting.aabb.radii = { 10.0f, 3.0f, 6.0f };
		setting.portalPosition = { 0.0f, 0.5f, 12.0f };
		//setting.placementCandidates.emplace_back(RoomType::SIMPLE_ROOM_1);
		setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_1);
		setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_2);
		setting.placementCandidates.emplace_back(RoomType::END_ROOM);
		setting.placementCandidates.emplace_back(RoomType::PASSAGE_1);

		m_roomGenerateSettings.at(SIMPLE_ROOM_1) = setting;
	}

	// END_ROOM
	{
		RoomGenerateSetting setting;
		setting.weight = 10;
		setting.aabb.position = { 0.0f, 3.0f, 8.0f };
		setting.aabb.radii = { 6.0f, 3.0f, 6.0f };
		setting.portalPosition = { 0.0f, 0.5f, 12.0f };
		m_roomGenerateSettings.at(END_ROOM) = setting;
	}

	// CROSS_ROOM_1
	{
		RoomGenerateSetting setting;
		setting.weight = 30;
		setting.aabb.position = { 0.0f, 3.0f, 12.0f };
		setting.aabb.radii = { 10.0f, 3.0f, 10.0f };
		setting.portalPosition = { 0.0f, 0.5f, 12.0f };
		setting.placementCandidates.emplace_back(RoomType::SIMPLE_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_2);
		setting.placementCandidates.emplace_back(RoomType::END_ROOM);
		setting.placementCandidates.emplace_back(RoomType::PASSAGE_1);
		m_roomGenerateSettings.at(CROSS_ROOM_1) = setting;
	}

	// CROSS_ROOM_2
	{
		RoomGenerateSetting setting;
		setting.weight = 30;
		setting.aabb.position = { 0.0f, 3.0f, 12.0f };
		setting.aabb.radii = { 10.0f, 3.0f, 10.0f };
		setting.portalPosition = { 0.0f, 0.5f, 12.0f };
		setting.placementCandidates.emplace_back(RoomType::SIMPLE_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_2);
		setting.placementCandidates.emplace_back(RoomType::END_ROOM);
		setting.placementCandidates.emplace_back(RoomType::PASSAGE_1);
		m_roomGenerateSettings.at(CROSS_ROOM_2) = setting;
	}

	// PASSAGE_1
	{
		RoomGenerateSetting setting;
		setting.weight = 20;
		setting.aabb.position = { 0.0f, 4.5f, 10.0f };
		setting.aabb.radii = { 1.0f, 4.5f, 5.0f };
		setting.portalPosition = { 0.0f, 3.5f, 20.0f };
		setting.placementCandidates.emplace_back(RoomType::SIMPLE_ROOM_1);
		setting.placementCandidates.emplace_back(RoomType::END_ROOM);
		setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_1);
		setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_2);
		setting.placementCandidates.emplace_back(RoomType::PASSAGE_1);
		m_roomGenerateSettings.at(PASSAGE_1) = setting;
	}

	// DEAD_END
	{
		RoomGenerateSetting setting;
		setting.weight = 0;
		setting.aabb.position = { 0.0f, 0.0f, 0.0f };
		setting.aabb.radii = { 0.0f, 0.0f, 0.0f };
		m_roomGenerateSettings.at(DEAD_END) = setting;
	}



	// ダンジョンの生成設定
	m_dungeonGenerateSettings.maxDepth = 4;
}