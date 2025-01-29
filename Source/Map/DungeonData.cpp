#include "DungeonData.h"

DungeonData::DungeonData()
{
	// 初期化
	Initialize();
}

void DungeonData::Initialize()
{
	// 各種初期化
	// ファイル名は後の初期化処理で使用するため、必ず最初に呼び出す
	InitFileNames();

	m_currentFloor = 1;

	// Dungeon→Floor→Roomの順番で呼び出す
	InitDungeonGenSetting();
	InitFloorGenSettings();
	InitRoomGenSettings();

	InitModelFileDatas();
	InitCollisionFileDatas();

	InitTileDatas();
}

void DungeonData::InitDungeonGenSetting()
{
	m_dungeonGenerateSetting.maxFloor = 4;
}

void DungeonData::InitFloorGenSettings()
{
	// 255階くらいまでは一応リサイズしておく
	m_floorGenerateSettings.resize(255);

	// 将来的にjson読み込みにする
	m_floorGenerateSettings.at(0).maxDepth = 5;
	m_floorGenerateSettings.at(0).startRoomType = RoomType::TUTO_START;
	m_floorGenerateSettings.at(0).endRoomType = RoomType::TUTO_END;
	m_floorGenerateSettings.at(0).deadEndRoomType = RoomType::TUTO_DEAD_END;

	// FirstFloor
	m_floorGenerateSettings.at(1).maxDepth = 3;
	m_floorGenerateSettings.at(1).startRoomType = RoomType::FIRST_START;
	m_floorGenerateSettings.at(1).endRoomType = RoomType::FIRST_END;
	m_floorGenerateSettings.at(1).deadEndRoomType = RoomType::FIRST_DEAD_END;
	//m_floorGenerateSettings.at(2).maxDepth = 4;
	//m_floorGenerateSettings.at(2).startRoomType = RoomType::FIRST_START;
	//m_floorGenerateSettings.at(2).endRoomType = RoomType::FIRST_END;
	//m_floorGenerateSettings.at(2).deadEndRoomType = RoomType::FIRST_DEAD_END;

	// SecondFloor
	m_floorGenerateSettings.at(2).maxDepth = 3;
	m_floorGenerateSettings.at(2).startRoomType = RoomType::SECOND_START;
	m_floorGenerateSettings.at(2).endRoomType = RoomType::SECOND_END;
	m_floorGenerateSettings.at(2).deadEndRoomType = RoomType::SECOND_DEAD_END;
	//m_floorGenerateSettings.at(4).maxDepth = 4;
	//m_floorGenerateSettings.at(4).startRoomType = RoomType::SECOND_START;
	//m_floorGenerateSettings.at(4).endRoomType = RoomType::SECOND_END;
	//m_floorGenerateSettings.at(4).deadEndRoomType = RoomType::SECOND_DEAD_END;

	// ThirdFloor
	m_floorGenerateSettings.at(3).maxDepth = 4;
	m_floorGenerateSettings.at(3).startRoomType = RoomType::THIRD_START;
	m_floorGenerateSettings.at(3).endRoomType = RoomType::FIRST_END;
	m_floorGenerateSettings.at(3).deadEndRoomType = RoomType::FIRST_DEAD_END;
	//m_floorGenerateSettings.at(6).maxDepth = 4;
	//m_floorGenerateSettings.at(6).startRoomType = RoomType::FIRST_START;
	//m_floorGenerateSettings.at(6).endRoomType = RoomType::FIRST_END;
	//m_floorGenerateSettings.at(6).deadEndRoomType = RoomType::FIRST_DEAD_END;

	// BossFloor
	// endRoomTypeが該当する部屋にテレポーターを配置するため、FIRST_BOSSを両方に適応させる
	m_floorGenerateSettings.at(4).startRoomType = RoomType::FIRST_BOSS;
	m_floorGenerateSettings.at(4).endRoomType = RoomType::FIRST_BOSS;
}

void DungeonData::InitRoomGenSettings()
{
	// 部屋タイプの数で配列をリサイズする
	m_roomGenerateSettings.resize(ROOMTYPE_COUNT);

	for (int i = (int)RoomType::FIRST_START; i < (int)RoomType::ROOMTYPE_COUNT; i++)
	{
		m_roomGenerateSettings.at((RoomType)i) = LoadRoomGenSetting((RoomType(i)));
	}

	/*///// First Floor /////*/
	m_roomGenerateSettings.at(FIRST_START).placementCandidates.emplace_back(RoomType::FIRST_I);
	m_roomGenerateSettings.at(FIRST_START).placementCandidates.emplace_back(RoomType::FIRST_T);
	m_roomGenerateSettings.at(FIRST_START).placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
	m_roomGenerateSettings.at(FIRST_START).placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);

	m_roomGenerateSettings.at(FIRST_I).weight = 10;
	m_roomGenerateSettings.at(FIRST_I).placementCandidates.emplace_back(RoomType::FIRST_I);
	m_roomGenerateSettings.at(FIRST_I).placementCandidates.emplace_back(RoomType::FIRST_T);
	m_roomGenerateSettings.at(FIRST_I).placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
	m_roomGenerateSettings.at(FIRST_I).placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);

	m_roomGenerateSettings.at(FIRST_T).weight = 20;
	m_roomGenerateSettings.at(FIRST_T).placementCandidates.emplace_back(RoomType::FIRST_I);
	m_roomGenerateSettings.at(FIRST_T).placementCandidates.emplace_back(RoomType::FIRST_T);
	m_roomGenerateSettings.at(FIRST_T).placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
	m_roomGenerateSettings.at(FIRST_T).placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);

	m_roomGenerateSettings.at(FIRST_FOUNTAIN).weight = 5;
	m_roomGenerateSettings.at(FIRST_FOUNTAIN).placementCandidates.emplace_back(RoomType::FIRST_I);
	m_roomGenerateSettings.at(FIRST_FOUNTAIN).placementCandidates.emplace_back(RoomType::FIRST_T);
	//m_roomGenerateSettings.at(FIRST_FOUNTAIN).placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
	m_roomGenerateSettings.at(FIRST_FOUNTAIN).placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);

	m_roomGenerateSettings.at(FIRST_SPAWNER).weight = 10;
	m_roomGenerateSettings.at(FIRST_SPAWNER).placementCandidates.emplace_back(RoomType::FIRST_I);
	m_roomGenerateSettings.at(FIRST_SPAWNER).placementCandidates.emplace_back(RoomType::FIRST_T);
	m_roomGenerateSettings.at(FIRST_SPAWNER).placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
	//m_roomGenerateSettings.at(FIRST_SPAWNER).placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);

	/*///// Second Floor /////*/
	m_roomGenerateSettings.at(SECOND_START).placementCandidates.emplace_back(RoomType::SECOND_L1);
	m_roomGenerateSettings.at(SECOND_START).placementCandidates.emplace_back(RoomType::SECOND_L2);
	m_roomGenerateSettings.at(SECOND_START).placementCandidates.emplace_back(RoomType::SECOND_CROSS);

	m_roomGenerateSettings.at(SECOND_L1).weight = 10;
	m_roomGenerateSettings.at(SECOND_L1).placementCandidates.emplace_back(RoomType::SECOND_L1);
	m_roomGenerateSettings.at(SECOND_L1).placementCandidates.emplace_back(RoomType::SECOND_L2);
	m_roomGenerateSettings.at(SECOND_L1).placementCandidates.emplace_back(RoomType::SECOND_CROSS);

	m_roomGenerateSettings.at(SECOND_L2).weight = 10;
	m_roomGenerateSettings.at(SECOND_L2).placementCandidates.emplace_back(RoomType::SECOND_L1);
	m_roomGenerateSettings.at(SECOND_L2).placementCandidates.emplace_back(RoomType::SECOND_L2);
	m_roomGenerateSettings.at(SECOND_L2).placementCandidates.emplace_back(RoomType::SECOND_CROSS);

	m_roomGenerateSettings.at(SECOND_CROSS).weight = 5;
	m_roomGenerateSettings.at(SECOND_CROSS).placementCandidates.emplace_back(RoomType::SECOND_L1);
	m_roomGenerateSettings.at(SECOND_CROSS).placementCandidates.emplace_back(RoomType::SECOND_L2);
	//m_roomGenerateSettings.at(SECOND_CROSS).placementCandidates.emplace_back(RoomType::SECOND_CROSS);

	/*///// Third Floor /////*/
	m_roomGenerateSettings.at(THIRD_START).placementCandidates.emplace_back(RoomType::THIRD_T);
	m_roomGenerateSettings.at(THIRD_START).placementCandidates.emplace_back(RoomType::THIRD_I);
	m_roomGenerateSettings.at(THIRD_START).placementCandidates.emplace_back(RoomType::THIRD_BARREL);

	m_roomGenerateSettings.at(THIRD_I).weight = 5;
	m_roomGenerateSettings.at(THIRD_I).placementCandidates.emplace_back(RoomType::THIRD_T);
	m_roomGenerateSettings.at(THIRD_I).placementCandidates.emplace_back(RoomType::THIRD_I);
	m_roomGenerateSettings.at(THIRD_I).placementCandidates.emplace_back(RoomType::THIRD_BARREL);

	m_roomGenerateSettings.at(THIRD_T).weight = 10;
	m_roomGenerateSettings.at(THIRD_T).placementCandidates.emplace_back(RoomType::THIRD_T);
	m_roomGenerateSettings.at(THIRD_T).placementCandidates.emplace_back(RoomType::THIRD_I);
	m_roomGenerateSettings.at(THIRD_T).placementCandidates.emplace_back(RoomType::THIRD_BARREL);

	m_roomGenerateSettings.at(THIRD_BARREL).weight = 10;
	m_roomGenerateSettings.at(THIRD_BARREL).placementCandidates.emplace_back(RoomType::THIRD_T);
	m_roomGenerateSettings.at(THIRD_BARREL).placementCandidates.emplace_back(RoomType::THIRD_I);
	m_roomGenerateSettings.at(THIRD_BARREL).placementCandidates.emplace_back(RoomType::THIRD_BARREL);



//	// DEAD_END
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 0;
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(DEAD_END));
//
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//
//			ifs.close();
//		}
//		
//		m_roomGenerateSettings.at(DEAD_END) = setting;
//	}
//
//	// FIRST_START
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 0;
//		setting.placementCandidates.emplace_back(RoomType::FIRST_I);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_T);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_START));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			// 接続点データの取り出し
//			for (const auto& nodeData : loadFile["NodeDatas"])
//			{
//				TileType tileType = nodeData["Type"];
//
//				if (tileType == CONNECTPOINT)
//				{
//					TILE_DATA newConnectPoint;
//					newConnectPoint.position = {
//						nodeData["Position"].at(0),
//						nodeData["Position"].at(1),
//						nodeData["Position"].at(2)
//					};
//					newConnectPoint.angle = {
//						nodeData["Angle"].at(0),
//						nodeData["Angle"].at(1),
//						nodeData["Angle"].at(2)
//					};
//					setting.connectPointDatas.emplace_back(newConnectPoint);
//				}
//			}
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_START) = setting;
//	}
//
//	// FIRST_I
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 20;
//		setting.placementCandidates.emplace_back(RoomType::FIRST_I);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_T);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_I));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			// 接続点データの取り出し
//			for (const auto& nodeData : loadFile["NodeDatas"])
//			{
//				TileType tileType = nodeData["Type"];
//
//				if (tileType == CONNECTPOINT)
//				{
//					TILE_DATA newConnectPoint;
//					newConnectPoint.position = {
//						nodeData["Position"].at(0),
//						nodeData["Position"].at(1),
//						nodeData["Position"].at(2)
//					};
//					newConnectPoint.angle = {
//						nodeData["Angle"].at(0),
//						nodeData["Angle"].at(1),
//						nodeData["Angle"].at(2)
//					};
//					setting.connectPointDatas.emplace_back(newConnectPoint);
//				}
//			}
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_I) = setting;
//	}
//
//	// FIRST_T
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 20;
//		setting.placementCandidates.emplace_back(RoomType::FIRST_I);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_T);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_T));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			// 接続点データの取り出し
//			for (const auto& nodeData : loadFile["NodeDatas"])
//			{
//				TileType tileType = nodeData["Type"];
//
//				if (tileType == CONNECTPOINT)
//				{
//					TILE_DATA newConnectPoint;
//					newConnectPoint.position = {
//						nodeData["Position"].at(0),
//						nodeData["Position"].at(1),
//						nodeData["Position"].at(2)
//					};
//					newConnectPoint.angle = {
//						nodeData["Angle"].at(0),
//						nodeData["Angle"].at(1),
//						nodeData["Angle"].at(2)
//					};
//					setting.connectPointDatas.emplace_back(newConnectPoint);
//				}
//			}
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_T) = setting;
//	}
//
//	// FIRST_FOUNTAIN
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 10;
//		setting.placementCandidates.emplace_back(RoomType::FIRST_I);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_T);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_SPAWNER);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_FOUNTAIN));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			// 接続点データの取り出し
//			for (const auto& nodeData : loadFile["NodeDatas"])
//			{
//				TileType tileType = nodeData["Type"];
//
//				if (tileType == CONNECTPOINT)
//				{
//					TILE_DATA newConnectPoint;
//					newConnectPoint.position = {
//						nodeData["Position"].at(0),
//						nodeData["Position"].at(1),
//						nodeData["Position"].at(2)
//					};
//					newConnectPoint.angle = {
//						nodeData["Angle"].at(0),
//						nodeData["Angle"].at(1),
//						nodeData["Angle"].at(2)
//					};
//					setting.connectPointDatas.emplace_back(newConnectPoint);
//				}
//			}
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_FOUNTAIN) = setting;
//	}
//
//	// FIRST_SPAWNER
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 10;
//		setting.placementCandidates.emplace_back(RoomType::FIRST_I);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_T);
//		setting.placementCandidates.emplace_back(RoomType::FIRST_FOUNTAIN);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_SPAWNER));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			// 接続点データの取り出し
//			for (const auto& nodeData : loadFile["NodeDatas"])
//			{
//				TileType tileType = nodeData["Type"];
//
//				if (tileType == CONNECTPOINT)
//				{
//					TILE_DATA newConnectPoint;
//					newConnectPoint.position = {
//						nodeData["Position"].at(0),
//						nodeData["Position"].at(1),
//						nodeData["Position"].at(2)
//					};
//					newConnectPoint.angle = {
//						nodeData["Angle"].at(0),
//						nodeData["Angle"].at(1),
//						nodeData["Angle"].at(2)
//					};
//					setting.connectPointDatas.emplace_back(newConnectPoint);
//				}
//			}
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_SPAWNER) = setting;
//	}
//
//	// FIRST_END
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 5;
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_END));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_END) = setting;
//	}
//
//	// FIRST_BOSS
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 5;
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(FIRST_BOSS));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(FIRST_BOSS) = setting;
//	}
//
//	// TEST_I
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 10;
//		setting.placementCandidates.emplace_back(RoomType::TEST_I);
//		setting.placementCandidates.emplace_back(RoomType::TEST_T);
//		setting.placementCandidates.emplace_back(RoomType::TEST_X);
//		//setting.placementCandidates.emplace_back(RoomType::TEST_END);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(TEST_I));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(TEST_I) = setting;
//	}
//
//	// TEST_T
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 20;
//		setting.placementCandidates.emplace_back(RoomType::TEST_I);
//		setting.placementCandidates.emplace_back(RoomType::TEST_T);
//		setting.placementCandidates.emplace_back(RoomType::TEST_X);
//		//setting.placementCandidates.emplace_back(RoomType::TEST_END);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(TEST_T));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(TEST_T) = setting;
//	}
//
//	// TEST_X
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 5;
//		setting.placementCandidates.emplace_back(RoomType::TEST_I);
//		setting.placementCandidates.emplace_back(RoomType::TEST_T);
//		setting.placementCandidates.emplace_back(RoomType::TEST_X);
//		//setting.placementCandidates.emplace_back(RoomType::TEST_END);
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(TEST_X));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//
//		m_roomGenerateSettings.at(TEST_X) = setting;
//	}
//
//	// TEST_END
//	{
//		RoomGenerateSetting setting;
//		setting.weight = 5;
//
//		// ファイルロード
//		nlohmann::json loadFile;
//		std::ifstream ifs(m_fileNames.at(TEST_END));
//		if (ifs.is_open())
//		{
//			ifs >> loadFile;
//			setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
//			setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
//			setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
//			setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
//			setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
//			setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
//			ifs.close();
//		}
//		m_roomGenerateSettings.at(TEST_END) = setting;
//	}
//
//	// TUTO_END
//	{
//		RoomGenerateSetting setting;
//		setting.portalPosition = { 0.0f, 0.0f, 10.0f };
//		m_roomGenerateSettings.at(TUTO_END) = setting;
//	}
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
	m_modelFileDatas.at(FOUNTAIN).emplace_back("Data/Model/DungeonAssets/SM_Fountain_01.glb", 4.0f);
	m_modelFileDatas.at(STAIR_TO_NEXTFLOOR).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Steps_01a.glb", 4.0f);
	m_modelFileDatas.at(BOSSROOM).emplace_back("Data/Model/DungeonAssets/SM_BossRoom.glb", 4.0f);
	m_modelFileDatas.at(BARREL).emplace_back("Data/Model/DungeonAssets/SM_Barrel.glb", 4.0f);
	m_modelFileDatas.at(WALL_BRICK_01A).emplace_back("Data/Model/DungeonAssets/SM_Brick_Wall_01a.glb", 4.0f);
	m_modelFileDatas.at(CHANDELIER).emplace_back("Data/Model/DungeonAssets/SM_Chandelier.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_BROWN).emplace_back("Data/Model/DungeonAssets/SM_Floor_Brown.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_CHECK).emplace_back("Data/Model/DungeonAssets/SM_Floor_Check_Board.glb", 4.0f);
	m_modelFileDatas.at(FLOOR_GRAY).emplace_back("Data/Model/DungeonAssets/SM_Floor_Gray.glb", 4.0f);
	m_modelFileDatas.at(HEART_ARCH).emplace_back("Data/Model/DungeonAssets/SM_Heart_Arch_01a.glb", 4.0f);
	m_modelFileDatas.at(PLASTIC_FLOWER).emplace_back("Data/Model/DungeonAssets/SM_Plastic_Flower.glb", 4.0f);
	m_modelFileDatas.at(SET_01A).emplace_back("Data/Model/DungeonAssets/SM_Set_01a.glb", 4.0f);
	m_modelFileDatas.at(SET_01B).emplace_back("Data/Model/DungeonAssets/SM_Set_01b.glb", 4.0f);
	m_modelFileDatas.at(SET_01C).emplace_back("Data/Model/DungeonAssets/SM_Set_01c.glb", 4.0f);
	m_modelFileDatas.at(STAIR_STEP_01B).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Steps_01b.glb", 4.0f);
	m_modelFileDatas.at(TOY_ARCH_01A).emplace_back("Data/Model/DungeonAssets/SM_Toy_Arch_01a.glb", 4.0f);
	m_modelFileDatas.at(TOY_ARCH_SUPPORT_01A).emplace_back("Data/Model/DungeonAssets/SM_Toy_Arch_Support_01a.glb", 4.0f);
	m_modelFileDatas.at(TOY_ARCH_TOP_01A).emplace_back("Data/Model/DungeonAssets/SM_Toy_Arch_Top_01a.glb", 4.0f);
	m_modelFileDatas.at(WALL_PAPER).emplace_back("Data/Model/DungeonAssets/SM_Wall_Paper.glb", 4.0f);
	m_modelFileDatas.at(WALL_SQUARES).emplace_back("Data/Model/DungeonAssets/SM_Wall_Squares.glb", 4.0f);
	m_modelFileDatas.at(WELL).emplace_back("Data/Model/DungeonAssets/SM_Well_01a.glb", 4.0f);

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
	m_collisionFileDatas.at(ARCH_FLOOR_01A).emplace_back("Data/Model/DungeonAssets/FloorCollision_01a.glb", 4.0f);

	//m_collisionFileDatas.at(STAIR_RAILING_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Railing_01a.glb", 4.0f);
	//m_collisionFileDatas.at(STAIR_STEP_01A).emplace_back("Data/Model/DungeonAssets/SM_Stairs_Steps_01a.glb", 4.0f);
	m_collisionFileDatas.at(FOUNTAIN).emplace_back("Data/Model/DungeonAssets/FountainCollision.glb", 4.0f);
	m_collisionFileDatas.at(BOSSROOM).emplace_back("Data/Model/DungeonAssets/SM_BossRoom.glb", 4.0f);

	m_collisionFileDatas.at(WALL_BRICK_01A).emplace_back("Data/Model/DungeonAssets/SM_Brick_Wall_01a.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_BROWN).emplace_back("Data/Model/DungeonAssets/SM_Floor_Brown.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_CHECK).emplace_back("Data/Model/DungeonAssets/SM_Floor_Check_Board.glb", 4.0f);
	m_collisionFileDatas.at(FLOOR_GRAY).emplace_back("Data/Model/DungeonAssets/SM_Floor_Gray.glb", 4.0f);
	m_collisionFileDatas.at(HEART_ARCH).emplace_back("Data/Model/DungeonAssets/SM_Heart_Arch_01a.glb", 4.0f);
	m_collisionFileDatas.at(STAIR_STEP_01B).emplace_back("Data/Model/DungeonAssets/SlopeCollision_01a.glb", 4.0f);
	m_collisionFileDatas.at(TOY_ARCH_01A).emplace_back("Data/Model/DungeonAssets/ToyArchCollision.glb", 4.0f);
	m_collisionFileDatas.at(TOY_ARCH_SUPPORT_01A).emplace_back("Data/Model/DungeonAssets/ToyArchCollision.glb", 4.0f);
	m_collisionFileDatas.at(WALL_PAPER).emplace_back("Data/Model/DungeonAssets/SM_Wall_Paper.glb", 4.0f);
	m_collisionFileDatas.at(WALL_SQUARES).emplace_back("Data/Model/DungeonAssets/SM_Wall_Squares.glb", 4.0f);
	m_collisionFileDatas.at(WELL).emplace_back("Data/Model/DungeonAssets/SM_Well_01a.glb", 4.0f);
}

void DungeonData::InitTileDatas()
{
	// まずリサイズ
	m_roomTileDatas.resize(RoomType::ROOMTYPE_COUNT);
	for (int i = (int)RoomType::FIRST_START; i < (int)RoomType::ROOMTYPE_COUNT; i++) m_roomTileDatas.at(i).resize(TileType::TILETYPE_COUNT);

	m_roomSpawnerDatas.resize(RoomType::ROOMTYPE_COUNT);
	m_roomStairDatas.resize(RoomType::ROOMTYPE_COUNT);

	// 部屋の数だけぶん回す
	for (int i = (int)RoomType::FIRST_START; i < (int)RoomType::ROOMTYPE_COUNT; i++)
	{
		nlohmann::json loadFile;
		std::ifstream ifs(m_fileNames.at((RoomType)i));

		if (ifs.is_open())
		{
			ifs >> loadFile;

			for (nlohmann::json_abi_v3_11_3::json nodeData : loadFile["NodeDatas"])
			{
				TileType type = nodeData["Type"];

				switch (type)
				{
				case ns_RoomData::SPAWNER:				m_roomSpawnerDatas.at((RoomType)i).emplace_back(LoadSpawnerData(nodeData));	break;
				case ns_RoomData::STAIR_TO_NEXTFLOOR:	m_roomStairDatas.at((RoomType)i).emplace_back(LoadStairData(nodeData));	break;
				default: m_roomTileDatas.at((RoomType)i).at(type).emplace_back(LoadTileData(nodeData));	break;
				}
			}
			ifs.close();
		}
	}

	int a = 0;
}

TILE_DATA DungeonData::LoadTileData(nlohmann::json_abi_v3_11_3::json nodeData)
{
	DirectX::XMFLOAT3 position = {
		nodeData["Position"].at(0),
		nodeData["Position"].at(1),
		nodeData["Position"].at(2),
	};
	DirectX::XMFLOAT3 angle = {
		nodeData["Angle"].at(0),
		nodeData["Angle"].at(1),
		nodeData["Angle"].at(2),
	};
	DirectX::XMFLOAT3 scale = {
	nodeData["Scale"].at(0),
	nodeData["Scale"].at(1),
	nodeData["Scale"].at(2),
	};

	TILE_DATA tileData;
	tileData.position = position;
	tileData.angle = angle;
	tileData.scale = scale;

	return tileData;
}

SPAWNER_DATA DungeonData::LoadSpawnerData(nlohmann::json_abi_v3_11_3::json nodeData)
{
	SPAWNER_DATA spawnerData;
	if (!nodeData.is_null())
	{
		spawnerData.tileData.position = {
			nodeData["Position"].at(0),
			nodeData["Position"].at(1),
			nodeData["Position"].at(2),
		};
		spawnerData.tileData.angle = {
			nodeData["Angle"].at(0),
			nodeData["Angle"].at(1),
			nodeData["Angle"].at(2),
		};
		spawnerData.tileData.scale = {
			nodeData["Scale"].at(0),
			nodeData["Scale"].at(1),
			nodeData["Scale"].at(2),
		};
	}
	if (!nodeData["SpawnerData"].is_null())
	{
		spawnerData.enemyType = nodeData["SpawnerData"]["EnemyType"];
		spawnerData.searchRadius = nodeData["SpawnerData"]["SearchRadius"];
	}

	return spawnerData;
}

STAIR_TO_NEXTFLOOR_DATA DungeonData::LoadStairData(nlohmann::json_abi_v3_11_3::json nodeData)
{
	STAIR_TO_NEXTFLOOR_DATA stairToNextFloorData;
	if (!nodeData.is_null())
	{
		stairToNextFloorData.tileData.position = {
			nodeData["Position"].at(0),
			nodeData["Position"].at(1),
			nodeData["Position"].at(2),
		};
		stairToNextFloorData.tileData.angle = {
			nodeData["Angle"].at(0),
			nodeData["Angle"].at(1),
			nodeData["Angle"].at(2),
		};
		stairToNextFloorData.tileData.scale = {
			nodeData["Scale"].at(0),
			nodeData["Scale"].at(1),
			nodeData["Scale"].at(2),
		};
	}
	if (!nodeData["TeleporterData"].is_null())
	{
		stairToNextFloorData.portalTime = nodeData["TeleporterData"]["PortalTime"];
		stairToNextFloorData.interactionDistance = nodeData["TeleporterData"]["InteractionDistance"];
	}

	return stairToNextFloorData;
}

DungeonData::RoomGenerateSetting DungeonData::LoadRoomGenSetting(RoomType roomType)
{
	RoomGenerateSetting setting;
	setting.weight = 0;

	// ファイルロード
	nlohmann::json loadFile;
	std::ifstream ifs(m_fileNames.at((int)roomType));

	if (ifs.is_open())
	{
		ifs >> loadFile;
		setting.aabb.position.x = loadFile["RoomSetting"]["AABB"]["Position"].at(0);
		setting.aabb.position.y = loadFile["RoomSetting"]["AABB"]["Position"].at(1);
		setting.aabb.position.z = loadFile["RoomSetting"]["AABB"]["Position"].at(2);
		setting.aabb.radii.x = loadFile["RoomSetting"]["AABB"]["Radii"].at(0);
		setting.aabb.radii.y = loadFile["RoomSetting"]["AABB"]["Radii"].at(1);
		setting.aabb.radii.z = loadFile["RoomSetting"]["AABB"]["Radii"].at(2);
		ifs.close();
	}

	return setting;
}

void DungeonData::InitFileNames()
{
	// 部屋タイプの数で配列をリサイズする
	m_fileNames.resize(ROOMTYPE_COUNT);

	// 最初のフロア
	m_fileNames.at(FIRST_START) = (char*)("Data/RoomDatas/FirstFloor_Start.json");
	m_fileNames.at(FIRST_I) = (char*)("Data/RoomDatas/FirstFloor_I.json");
	m_fileNames.at(FIRST_T) = (char*)("Data/RoomDatas/FirstFloor_T.json");
	m_fileNames.at(FIRST_FOUNTAIN) = (char*)("Data/RoomDatas/FirstFloor_Fountain.json");
	m_fileNames.at(FIRST_SPAWNER) = (char*)("Data/RoomDatas/FirstFloor_Spawner.json");
	m_fileNames.at(FIRST_END) = (char*)("Data/RoomDatas/FirstFloor_End.json");
	m_fileNames.at(FIRST_BOSS) = (char*)("Data/RoomDatas/FirstFloor_Boss.json");
	m_fileNames.at(FIRST_DEAD_END) = (char*)("Data/RoomDatas/FirstFloor_DeadEnd.json");

	// 第二のフロア
	m_fileNames.at(SECOND_START) = (char*)("Data/RoomDatas/SecondFloor_Start.json");
	m_fileNames.at(SECOND_L1) = (char*)("Data/RoomDatas/SecondFloor_L1.json");
	m_fileNames.at(SECOND_L2) = (char*)("Data/RoomDatas/SecondFloor_L2.json");
	m_fileNames.at(SECOND_CROSS) = (char*)("Data/RoomDatas/SecondFloor_Cross.json");
	m_fileNames.at(SECOND_END) = (char*)("Data/RoomDatas/SecondFloor_End.json");
	m_fileNames.at(SECOND_DEAD_END) = (char*)("Data/RoomDatas/SecondFloor_DeadEnd.json");

	// 第三のフロア
	m_fileNames.at(THIRD_START) = (char*)("Data/RoomDatas/ThirdFloor_Start.json");
	m_fileNames.at(THIRD_I) = (char*)("Data/RoomDatas/ThirdFloor_I.json");
	m_fileNames.at(THIRD_T) = (char*)("Data/RoomDatas/ThirdFloor_T.json");
	m_fileNames.at(THIRD_BARREL) = (char*)("Data/RoomDatas/ThirdFloor_Barrel.json");
	m_fileNames.at(THIRD_END) = (char*)("Data/RoomDatas/ThirdFloor_Start.json");
	m_fileNames.at(THIRD_DEAD_END) = (char*)("Data/RoomDatas/ThirdFloor_Start.json");

	// チュートリアルフロア
	m_fileNames.at(TUTO_START) = (char*)("Data/RoomDatas/TutoFloor_Start.json");
	m_fileNames.at(TUTO_NOTHINGROOM) = (char*)("Data/RoomDatas/TutoFloor_Nothing.json");
	m_fileNames.at(TUTO_SPAWNERROOM) = (char*)("Data/RoomDatas/TutoFloor_Spawner.json");
	m_fileNames.at(TUTO_END) = (char*)("Data/RoomDatas/TutoFloor_End.json");
	m_fileNames.at(TUTO_DEAD_END) = (char*)("Data/RoomDatas/FirstFloor_DeadEnd.json");
}