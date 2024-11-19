#pragma once

#include "RoomBase.h"

class Passage1 : public RoomBase
{
public:
	// コンストラクタ（乱数による自動生成）
	Passage1(
		RoomBase* parent,
		int pointIndex,
		std::vector<AABB>& roomAABBs);

	// コンストラクタ（配列に基づいた生成）
	Passage1(
		RoomBase* parent,
		int pointIndex,
		std::vector<int> roomTree,
		int& treeIndex);

	void LoadMapTileData() override;

	void PlaceMapTile() override;
};