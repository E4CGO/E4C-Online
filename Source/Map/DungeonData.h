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
	enum RoomType
	{
		SIMPLE_ROOM_1 = 0,
		END_ROOM,
		CROSS_ROOM_1,
		CROSS_ROOM_2,
		PASSAGE_1,
		DEAD_END,
		ROOMTYPE_COUNT,
	};

	// タイルタイプ
	enum TileType
	{
		FLOOR = 0,
		WALL,
		PILLAR,
		STAIR,
		SPAWNER,
		PORTAL,
		TILETYPE_COUNT,
	};

	// 配置するタイルデータ
	struct TILE_DATA
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	// 接続点データ
	struct CONNECTPOINT_DATA
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4X4 transform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
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
		int weight;	// 重み、値が大きいほど生成確率が高くなる
		AABB aabb;	// AABB、部屋同士の当たり判定などに使用
		DirectX::XMFLOAT3 portalPosition;			// ポータル配置座標
		std::vector<RoomType> placementCandidates;	// 配置候補の部屋タイプを保存する配列
	};

	// ダンジョンの生成設定
	struct DungeonGenerateSetting
	{
		int maxDepth;	// 最大深度、親からの距離（深度）がこの値以上になった場合、子の生成をキャンセルする
	};

	// 部屋の生成設定を取得
	RoomGenerateSetting GetRoomGenerateSetting(RoomType type) { return m_roomGenerateSettings.at(static_cast<int>(type)); }
	// ダンジョンの生成設定を取得
	DungeonGenerateSetting GetDungeonGenerateSetting() { return m_dungeonGenerateSettings; }

private:
	std::vector<RoomGenerateSetting> m_roomGenerateSettings;	// 部屋の生成設定配列
	DungeonGenerateSetting m_dungeonGenerateSettings;			// ダンジョンの生成設定
};

#endif // !__INCLUDED_DUNGEON_DATA_H__