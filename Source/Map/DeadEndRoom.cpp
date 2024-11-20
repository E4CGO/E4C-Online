﻿#include "DeadEndRoom.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

DeadEndRoom::DeadEndRoom(
	RoomBase* parent, const int pointIndex,
	std::vector<AABB>& roomAABBs,
	const bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
	: RoomBase(parent, pointIndex, roomAABBs, isAutoGeneration, roomOrder, orderIndex)
{
	// 部屋タイプを設定
	roomType = DungeonData::DEAD_END;

	// 生成を行う（接続点がないため次の部屋は生成されない）
	GenerateNextRoom(
		roomAABBs,
		isAutoGeneration,
		roomOrder, orderIndex);
}

void DeadEndRoom::LoadMapData()
{
	// 何も置かない
}