﻿//! @file DungeonData.h
//! @note

//#pragma once

#ifndef __INCLUDED_DUNGEON_DATA_H__
#define __INCLUDED_DUNGEON_DATA_H__

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Physics/Collision.h"
#include <vector>

// 部屋データ
namespace ns_RoomData
{
	// 部屋タイプ
	enum RoomType : uint8_t
	{
		FIRST_START,	// 最初のフロア：最初の部屋
		FIRST_I,		// 最初のフロア：Ｉ字の部屋
		FIRST_T,		// 最初のフロア：Ｔ字の部屋
		FIRST_FOUNTAIN,	// 最初のフロア：噴水の部屋
		FIRST_SPAWNER,	// 最初のフロア：スポナーの部屋
		FIRST_END,		// 最初のフロア：最後の部屋
		FIRST_BOSS,		// 最初のフロア：ボス部屋
		FIRST_DEAD_END,	// 最初のフロア：行き止まり

		SECOND_START,		// 第二のフロア：最初の部屋
		SECOND_L1,			// 第二のフロア：Ⅼ字の部屋・右向き
		SECOND_L2,			// 第二のフロア：Ⅼ字の部屋・左向き
		SECOND_CROSS,		// 第二のフロア：十字の部屋
		SECOND_END,			// 第二のフロア：最後の部屋
		SECOND_DEAD_END,	// 第二のフロア：行き止まり

		THIRD_START,		// 第三のフロア：最初の部屋
		THIRD_I,			// 第三のフロア：Ｉ字の部屋
		THIRD_T,			// 第三のフロア：Ｔ字の部屋
		THIRD_BARREL,		// 第三のフロア：タルの部屋
		THIRD_END,			// 第三のフロア：最後の部屋
		THIRD_DEAD_END,		// 第三のフロア：行き止まり

		TUTO_START,			// チュートリアル：最初の部屋
		TUTO_NOTHINGROOM,	// チュートリアル：何もない部屋
		TUTO_SPAWNERROOM,	// チュートリアル：スポナーのある部屋
		TUTO_END,			// チュートリアル：最後の部屋
		TUTO_DEAD_END,		// チュートリアル：行き止まり

		ROOMTYPE_COUNT
	};

	// タイルタイプ
	enum TileType : uint8_t
	{
		FLOOR_01A = 0,
		FLOOR_01B,
		FLOOR_02A,
		FLOOR_03A,
		FLOOR_CLOUD_01A,
		WALL_01A,
		WALL_01B,
		WALL_02A,
		WALL_02B,
		WALL_03A,
		WALL_04A,
		WALL_CLOUD,
		ARCH_01A,
		ARCH_ENTRANCE_01A,
		ARCH_FLOOR_01A,
		STAIR_RAILING_01A,
		STAIR_STEP_01A,
		CARAMEL_01,
		CARAMEL_02,
		CLOUD_01,
		CLOUD_02,
		CREAM_01,
		CREAM_02,
		LOLIPOP_01A,
		LOLIPOP_01B,
		LOLIPOP_02A,
		STAR,
		FIRE_HYDRANT,
		PORTAL,
		SPAWNER,
		CONNECTPOINT,
		FOUNTAIN,
		STAIR_TO_NEXTFLOOR,
		BOSSROOM,
		BARREL,
		WALL_BRICK_01A,
		CHANDELIER,
		FLOOR_BROWN,
		FLOOR_CHECK,
		FLOOR_GRAY,
		HEART_ARCH,
		PLASTIC_FLOWER,
		SET_01A,
		SET_01B,
		SET_01C,
		STAIR_STEP_01B,
		TOY_ARCH_01A,
		TOY_ARCH_SUPPORT_01A,
		TOY_ARCH_TOP_01A,
		WALL_PAPER,
		WALL_SQUARES,
		WELL,

		TILETYPE_COUNT
	};

	// スポナーデータ
	struct SPAWNER_DATA
	{
		uint8_t enemyType = 0;

		float searchRadius = 5.0f;
		float spawnRadius = 3.0f;

		int maxExistedEnemiesNum = 1;
		int maxSpawnedEnemiesNum = -1;

		float spawnTime = 2.0f;
	};

	// 配置するタイルデータ
	struct TILE_DATA
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	// モデルファイル読み込み用データ
	struct FILE_DATA
	{
		std::string fileName = "";
		float scale = 1.0f;
	};
}

using namespace ns_RoomData;

/**************************************************************************//**
	@class		DungeonData
	@brief		ダンジョン生成に必要なデータを保持するクラス
	@par		[説明]
					
*//***************************************************************************/
class DungeonData : public Singleton<DungeonData>
{
	friend class Singleton<DungeonData>;
public:
	DungeonData();
	~DungeonData() {}

public:	
	// 部屋の生成設定
	struct RoomGenerateSetting
	{
		int weight = 0;	// 重み 値が大きいほど生成確率が高くなる
		AABB aabb {};	// AABB 部屋同士の当たり判定などに使用
		std::vector<TILE_DATA> connectPointDatas;	// 接続点データ配列
		std::vector<RoomType> placementCandidates;	// 配置候補の部屋タイプを保存する配列
	};

	// 階全体の生成設定
	struct FloorGenerateSetting
	{
		int maxDepth = 0;	// 最大深度 親からの距離（深度）がこの値以上になった場合、子の生成をキャンセルする
		RoomType startRoomType = RoomType::TUTO_START;	// 最初に生成する部屋タイプ
		RoomType endRoomType = RoomType::TUTO_END;		// 最後に生成する部屋タイプ
		RoomType deadEndRoomType = RoomType::TUTO_DEAD_END;	// 行き止まりに生成する部屋タイプ
	};

	// ダンジョンの生成設定
	struct DungeonGenerateSetting
	{
		int maxFloor = 3;	// 最大階数　最上階には次の階への階段などは設置しない
	};

	// 初期化
	void Initialize();

	// ダンジョンの生成設定を取得
	DungeonGenerateSetting GetDungeonGenSetting() { return m_dungeonGenerateSetting; }
	// 階全体の生成設定を配列から取得
	FloorGenerateSetting GetFloorGenSetting(int floor) { return m_floorGenerateSettings.at(floor); }
	// 現在の階の生成設定を取得
	FloorGenerateSetting GetCurrentFloorGenSetting() { return m_floorGenerateSettings.at(m_currentFloor); }
	// 部屋の生成設定を配列から取得
	RoomGenerateSetting GetRoomGenSetting(RoomType type) { return m_roomGenerateSettings.at(static_cast<int>(type)); }
	// 現在の階取得・設定・次の階へ
	const int GetCurrentFloor() const { return m_currentFloor; }
	void SetCurrentFloor(int floor) { m_currentFloor = floor; }
	void GoToNextFloor() { m_currentFloor++; }

	// ファイル読み込み用データの取得
	const std::vector<FILE_DATA> GetModelFileDatas(TileType type) const { return m_modelFileDatas.at(type); }
	const std::vector<FILE_DATA> GetCollisionFileDatas(TileType type) const { return m_collisionFileDatas.at(type); }
	// ファイル名の取得
	const char* GetFileName(RoomType type) { return m_fileNames.at(type); }

private:
	// 各種初期化
	void InitFileNames();	// 最初に呼び出す
	void InitDungeonGenSetting();	// 二番目に呼び出す
	void InitFloorGenSettings();	// 三番目に〃
	void InitRoomGenSettings();	// 四番目に〃
	void InitModelFileDatas();
	void InitCollisionFileDatas();

	// jsonデータ読み込み
	RoomGenerateSetting LoadRoomGenSetting(RoomType roomType);

	std::vector<RoomGenerateSetting> m_roomGenerateSettings;	// 部屋の生成設定配列
	std::vector<FloorGenerateSetting> m_floorGenerateSettings;	// 階全体の生成設定配列
	DungeonGenerateSetting m_dungeonGenerateSetting;			// ダンジョンの生成設定
	int m_currentFloor = 1;										// 現在の階
	std::vector<std::vector<FILE_DATA>> m_modelFileDatas;		// 見た目用ファイル読み込み用データ配列
	std::vector<std::vector<FILE_DATA>> m_collisionFileDatas;	// 当たり判定用ファイル読み込み用データ配列
	std::vector<char*> m_fileNames;						// ファイル名配列
};
#define DUNGEONDATA DungeonData::Instance()

#endif // !__INCLUDED_DUNGEON_DATA_H__