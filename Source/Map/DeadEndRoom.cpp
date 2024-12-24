#include "DeadEndRoom.h"

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
	roomType = RoomType::DEAD_END;

	// 生成を行う（接続点がないため次の部屋は生成されない）
	GenerateNextRoom(
		roomAABBs,
		isAutoGeneration,
		roomOrder, orderIndex);
}

void DeadEndRoom::LoadMapData()
{
	//// 行き止まりの壁だけ設置する
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -2.0f, 0.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -2.0f, 3.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
}