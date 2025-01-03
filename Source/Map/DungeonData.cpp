#include "DungeonData.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Network/WinSock2Wrapper.h"

#include <fstream>

DungeonData::DungeonData()
{
	// 初期化
	Initialize();
}

void DungeonData::Initialize()
{
	// 各種初期化
	InitRoomGenerateSettings();
	InitDungeonGenerateSetting();
	InitModelFileDatas();
	InitCollisionFileDatas();

	m_dungeonGenerateSetting.maxDepth = 4;
}

void DungeonData::InitRoomGenerateSettings()
{
	// 部屋タイプの数で配列をリサイズする
	m_roomGenerateSettings.resize(ROOMTYPE_COUNT);

	// SIMPLE_ROOM_1
	{
		RoomGenerateSetting setting;
		setting.weight = 50;
		setting.aabb.position = { 0.0f, 3.0f, 8.0f };
		setting.aabb.radii = { 10.0f, 3.0f, 6.0f };
		setting.portalPosition = { 0.0f, 10000.0f, 0.0f };
		setting.placementCandidates.emplace_back(RoomType::SIMPLE_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_1);
		//setting.placementCandidates.emplace_back(RoomType::CROSS_ROOM_2);
		//setting.placementCandidates.emplace_back(RoomType::END_ROOM);
		//setting.placementCandidates.emplace_back(RoomType::PASSAGE_1);

		// ファイルロード
		nlohmann::json loadFile;
		std::ifstream ifs("Data/RoomDatas/SimpleRoom1.json");

		if (ifs.is_open())
		{
			ifs >> loadFile;

			//setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
			//setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
			//setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
			//setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
			//setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
			//setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);

			ifs.close();
		}

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
}

void DungeonData::InitDungeonGenerateSetting()
{
	m_dungeonGenerateSetting.maxDepth = 1;
}

void DungeonData::InitModelFileDatas()
{
	// タイルタイプの数で配列をリサイズ
	m_modelFileDatas.resize(TILETYPE_COUNT);

	m_modelFileDatas.at(FLOOR_01A).emplace_back("Data/Model/DungeonAssets/SM_Floor_01a.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_01B).emplace_back("Data/Model/DungeonAssets/SM_Floor_01b.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_02A).emplace_back("Data/Model/DungeonAssets/SM_Floor_02a.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_03A).emplace_back("Data/Model/DungeonAssets/SM_Floor_03a.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_CLOUD_01A).emplace_back("Data/Model/DungeonAssets/SM_Floor_Cloud_01a.glb", 4.0f);

	m_modelFileDatas.at(WALL_01A).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_01a.glb", 4.0f);
	m_modelFileDatas.at(WALL_01B).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_01b.glb", 4.0f);
	m_modelFileDatas.at(WALL_02A).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_02a.glb", 4.0f);
	m_modelFileDatas.at(WALL_02B).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_02b.glb", 4.0f);
	m_modelFileDatas.at(WALL_03A).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_03a.glb", 4.0f);
	m_modelFileDatas.at(WALL_04A).emplace_back("Data/Model/DungeonAssets/SM_Wall_Pattern_04a.glb", 4.0f);
	m_modelFileDatas.at(WALL_CLOUD).emplace_back("Data/Model/DungeonAssets/SM_Wall_Cloud.glb", 4.0f);

	m_modelFileDatas.at(ARCH_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_01a.glb", 4.0f);
	m_modelFileDatas.at(ARCH_ENTRANCE_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_Entrance_01a.glb", 4.0f);
	m_modelFileDatas.at(ARCH_FLOOR_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_Floor_01a.glb", 4.0f);

	m_modelFileDatas.at(STAIR_RAILING_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Railing_01a.glb", 4.0f);
	m_modelFileDatas.at(STAIR_STEP_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Steps_01a.glb", 4.0f);

	m_modelFileDatas.at(CARAMEL_01).emplace_back("Data/Model/DungeonAssets/SM_Caramel_01.glb", 4.0f);
	m_modelFileDatas.at(CARAMEL_02).emplace_back("Data/Model/DungeonAssets/SM_Caramel_02.glb", 4.0f);
	m_modelFileDatas.at(CLOUD_01).emplace_back("Data/Model/DungeonAssets/SM_Cloud_01.glb", 4.0f);
	m_modelFileDatas.at(CLOUD_02).emplace_back("Data/Model/DungeonAssets/SM_Cloud_02.glb", 4.0f);
	m_modelFileDatas.at(CREAM_01).emplace_back("Data/Model/DungeonAssets/SM_Cream_01.glb", 4.0f);
	m_modelFileDatas.at(CREAM_02).emplace_back("Data/Model/DungeonAssets/SM_Cream_02.glb", 4.0f);
	m_modelFileDatas.at(LOLIPOP_01A).emplace_back("Data/Model/DungeonAssets/SM_Lollipop_01a.glb", 4.0f);
	m_modelFileDatas.at(LOLIPOP_01B).emplace_back("Data/Model/DungeonAssets/SM_Lollipop_01b.glb", 4.0f);
	m_modelFileDatas.at(LOLIPOP_02A).emplace_back("Data/Model/DungeonAssets/SM_Lollipop_02a.glb", 4.0f);
	m_modelFileDatas.at(STAR).emplace_back("Data/Model/DungeonAssets/SM_Star.glb", 4.0f);
	m_modelFileDatas.at(FIRE_HYDRANT).emplace_back("Data/Model/DungeonAssets/SM_Fire_Hydrant.glb", 4.0f);

	// モデルない組～
	m_modelFileDatas.at(PORTAL).emplace_back("", 0.0f);
	m_modelFileDatas.at(SPAWNER).emplace_back("", 0.0f);
	m_modelFileDatas.at(CONNECTPOINT).emplace_back("", 0.0f);
}

void DungeonData::InitCollisionFileDatas()
{
	// タイルタイプの数で配列をリサイズ
	m_collisionFileDatas.resize(TILETYPE_COUNT);

	m_collisionFileDatas.at(FLOOR_01A).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_01B).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_02A).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_03A).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_CLOUD_01A).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);

	m_collisionFileDatas.at(WALL_01A).emplace_back("Data/Model/DungeonAssets/WallCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(WALL_01B).emplace_back("Data/Model/DungeonAssets/WallCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(WALL_02A).emplace_back("Data/Model/DungeonAssets/WallCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(WALL_02B).emplace_back("Data/Model/DungeonAssets/WallCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(WALL_03A).emplace_back("Data/Model/DungeonAssets/WallCollision_01b.glb", 4.0f);
	m_collisionFileDatas.at(WALL_04A).emplace_back("Data/Model/DungeonAssets/WallCollision_01b.glb", 4.0f);
	m_collisionFileDatas.at(WALL_CLOUD).emplace_back("Data/Model/DungeonAssets/WallCollision_01b.glb", 4.0f);

	//m_collisionFileDatas.at(ARCH_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_01a.glb", 4.0f);
	//m_collisionFileDatas.at(ARCH_ENTRANCE_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_Entrance_01a.glb", 4.0f);
	//m_collisionFileDatas.at(ARCH_FLOOR_01A).emplace_back("Data/Model/DungeonAssets/SM_Arch_Floor_01a.glb", 4.0f);

	//m_collisionFileDatas.at(STAIR_RAILING_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Railing_01a.glb", 4.0f);
	//m_collisionFileDatas.at(STAIR_STEP_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Steps_01a.glb", 4.0f);
}