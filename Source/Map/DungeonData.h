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



	//// 部屋の生成配列を取得
	//std::vector<uint8_t> GetRoomTree() { return m_roomTree; }

	//// 次の生成配列内データを取得
	//uint8_t GetNextRoom()
	//{
	//	if (m_roomTreeIndex >= m_roomTree.size()) return DEAD_END;

	//	uint8_t ret = m_roomTree.at(m_roomTreeIndex);
	//	m_roomTreeIndex++;
	//	return ret;
	//}

	//// 部屋の生成配列を設定
	//void SetRoomTree(std::vector<uint8_t> newRoomTree) { m_roomTree = newRoomTree; }

	//// 部屋の生成配列に追加
	//void AddRoomTree(uint8_t newRoom) { m_roomTree.emplace_back(newRoom); }



	//// 部屋のAABB配列を取得
	//const std::vector<AABB>& GetRoomAABBs() { return m_roomAABBs; }

	//// 次のAABB配列内データを取得
	//AABB GetNextRoomAABB()
	//{
	//	AABB ret = m_roomAABBs.at(m_roomAABBsIndex);
	//	m_roomAABBsIndex++;
	//	return ret;
	//}

	//// 部屋のAABB配列を設定
	//void SetRoomAABBs(std::vector<AABB> newRoomAABBs) { m_roomAABBs = newRoomAABBs; }

	//// 部屋のAABB配列に追加
	//void AddRoomAABBs(AABB newAABB) { m_roomAABBs.emplace_back(newAABB); }



	// 部屋の生成設定を取得
	RoomGenerateSetting GetRoomGenerateSetting(RoomType type) { return m_roomGenerateSettings.at(static_cast<int>(type)); }

	// ダンジョンの生成設定を取得
	DungeonGenerateSetting GetDungeonGenerateSetting() { return m_dungeonGenerateSettings; }

private:
	//std::vector<uint8_t> m_roomTree;	// 部屋の生成配列
	//std::vector<AABB> m_roomAABBs;	// 部屋のAABB配列
	//int m_roomTreeIndex = 0;
	//int m_roomAABBsIndex = 0;
	std::vector<RoomGenerateSetting> m_roomGenerateSettings;	// 部屋の生成設定配列
	DungeonGenerateSetting m_dungeonGenerateSettings;			// ダンジョンの生成設定
};