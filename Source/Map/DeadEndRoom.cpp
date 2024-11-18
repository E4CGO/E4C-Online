#include "DeadEndRoom.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

DeadEndRoom::DeadEndRoom(
	RoomBase* parent,
	int pointIndex) : RoomBase(parent, pointIndex)
{
	// 部屋タイプを設定
	roomType = DungeonData::DEAD_END;

	// 生成を行う（接続点がないため次の部屋は生成されない）
	GenerateNextRoom();
}

void DeadEndRoom::LoadMapData()
{
	// 何も置かない
}