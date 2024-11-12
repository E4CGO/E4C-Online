﻿#pragma once

#include "RoomBase.h"

class CrossRoom1 : public RoomBase
{
public:
	// コンストラクタ（乱数による自動生成）
	CrossRoom1(
		RoomBase* parent,
		int pointIndex,
		std::vector<AABB>& roomAABBs);

	// コンストラクタ（配列に基づいた生成）
	CrossRoom1(
		RoomBase* parent,
		int pointIndex,
		std::vector<UINT16> roomTree,
		int& treeIndex);

	void LoadMapTileData() override;

	void PlaceMapTile() override;
};