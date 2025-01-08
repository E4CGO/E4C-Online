//! @file DungeonData.h
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
		SIMPLE_ROOM_1 = 0,
		END_ROOM,
		CROSS_ROOM_1,
		CROSS_ROOM_2,
		PASSAGE_1,
		DEAD_END,

		FIRST_START,	// 最初のフロア：最初の部屋
		FIRST_I,		// 最初のフロア：Ｉ字の部屋
		FIRST_T,		// 最初のフロア：Ｔ字の部屋
		FIRST_SPAWNER,	// 最初のフロア：スポナーの部屋
		FIRST_END,		// 最初のフロア：最後の部屋

		TEST_I,		// テスト：Ｉ字の部屋
		TEST_T,		// テスト：Ｔ字の部屋
		TEST_X,		// テスト：十字の部屋
		TEST_END,	// テスト：最後の部屋

		TUTO_START,			// チュートリアル：最初の部屋
		TUTO_NOTHINGROOM,	// チュートリアル：何もない部屋
		TUTO_SPAWNERROOM,	// チュートリアル：スポナーのある部屋
		TUTO_END,			// チュートリアル：最後の部屋

		ROOMTYPE_COUNT,
	};

	// タイルタイプ
	enum TileType : uint8_t
	{
		// 床タイル
		FLOOR_01A = 0,
		FLOOR_01B,
		FLOOR_02A,
		FLOOR_03A,
		FLOOR_CLOUD_01A,

		// 壁タイル
		WALL_01A,
		WALL_01B,
		WALL_02A,
		WALL_02B,
		WALL_03A,
		WALL_04A,
		WALL_CLOUD,

		// アーチタイル
		ARCH_01A,
		ARCH_ENTRANCE_01A,
		ARCH_FLOOR_01A,

		// 階段タイル
		STAIR_RAILING_01A,
		STAIR_STEP_01A,

		// 小物タイル
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

		// ゲームオブジェクトタイル
		PORTAL,
		SPAWNER,
		CONNECTPOINT,

		// enumCount
		TILETYPE_COUNT,
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
private:
	DungeonData();
	~DungeonData() {}

public:	
	// 部屋の生成設定
	struct RoomGenerateSetting
	{
		int weight = 0;	// 重み、値が大きいほど生成確率が高くなる
		AABB aabb {};	// AABB、部屋同士の当たり判定などに使用
		DirectX::XMFLOAT3 portalPosition {};		// ポータル配置座標
		std::vector<RoomType> placementCandidates;	// 配置候補の部屋タイプを保存する配列
	};

	// ダンジョンの生成設定
	struct DungeonGenerateSetting
	{
		int maxDepth;	// 最大深度、親からの距離（深度）がこの値以上になった場合、子の生成をキャンセルする
	};

	// 初期化
	void Initialize();
	// 各種初期化
	void InitRoomGenerateSettings();
	void InitDungeonGenerateSetting();
	void InitModelFileDatas();
	void InitCollisionFileDatas();
	void InitFileNames();

	// 部屋の生成設定を取得
	RoomGenerateSetting GetRoomGenerateSetting(RoomType type) { return m_roomGenerateSettings.at(static_cast<int>(type)); }
	// ダンジョンの生成設定を取得
	DungeonGenerateSetting GetDungeonGenerateSetting() { return m_dungeonGenerateSetting; }
	// ファイル読み込み用データの取得
	const std::vector<FILE_DATA> GetModelFileDatas(TileType type) const {
		int a = 0;
		int b = 0; return m_modelFileDatas.at(type); }
	const std::vector<FILE_DATA> GetCollisionFileDatas(TileType type) const { return m_collisionFileDatas.at(type); }
	// ファイル名の取得
	const char* GetFileName(RoomType type) { return m_fileNames.at(type); }

private:
	std::vector<RoomGenerateSetting> m_roomGenerateSettings;	// 部屋の生成設定配列
	DungeonGenerateSetting m_dungeonGenerateSetting;			// ダンジョンの生成設定
	std::vector<std::vector<FILE_DATA>> m_modelFileDatas;		// 見た目用ファイル読み込み用データ配列
	std::vector<std::vector<FILE_DATA>> m_collisionFileDatas;	// 当たり判定用ファイル読み込み用データ配列
	std::vector<char*> m_fileNames;						// ファイル名配列
};
#define DUNGEONDATA DungeonData::Instance()

#endif // !__INCLUDED_DUNGEON_DATA_H__