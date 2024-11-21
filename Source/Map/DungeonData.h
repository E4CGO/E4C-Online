#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Physics/Collision.h"
#include <vector>

// ダンジョンデータ

class DungeonData : public Singleton<DungeonData>
{
	friend class Singleton<DungeonData>;
private:
	DungeonData();
	~DungeonData() {}

public:
	// 部屋タイプ
	enum RoomType
	{
		SIMPLE_ROOM_1 = 0,
		END_ROOM,
		CROSS_ROOM_1,
		PASSAGE_1,
		DEAD_END,

		ENUM_COUNT
	};

	// 部屋の生成設定
	struct RoomGenerateSetting
	{
		int weight;	// 重み、多ければ多いほど生成確率が高い
		AABB aabb;	// 当たり判定用 AABB
		std::vector<RoomType> placementCandidates;			// 配置候補の部屋を保存する配列
	};

	// ダンジョンの生成設定
	struct DungeonGenerateSetting
	{
		int maxDepth;	// 最大深度、親からの距離（深度）がこの値以上になった場合、終端の部屋を生成する
	};



	// 部屋の生成設定を取得
	RoomGenerateSetting GetRoomGenerateSetting(RoomType type) { return m_roomGenerateSettings.at(static_cast<int>(type)); }

	// ダンジョンの生成設定を取得
	DungeonGenerateSetting GetDungeonGenerateSetting() { return m_dungeonGenerateSettings; }

private:
	std::vector<RoomGenerateSetting> m_roomGenerateSettings;	// 部屋の生成設定配列
	DungeonGenerateSetting m_dungeonGenerateSettings;			// ダンジョンの生成設定
};