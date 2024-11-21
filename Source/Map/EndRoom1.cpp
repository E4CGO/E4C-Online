#include "Map/EndRoom1.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

EndRoom1::EndRoom1(
	RoomBase* parent, const int pointIndex,
	std::vector<AABB>& roomAABBs,
	const bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
	: RoomBase(parent, pointIndex, roomAABBs, isAutoGeneration, roomOrder, orderIndex)
{
	// 部屋タイプを設定
	roomType = DungeonData::END_ROOM;

	// 生成を行う（接続点がないため次の部屋は生成されない）
	GenerateNextRoom(
		roomAABBs,
		isAutoGeneration,
		roomOrder, orderIndex);
}

void EndRoom1::LoadMapData()
{
	// 部屋タイプを設定
	roomType = DungeonData::END_ROOM;

	// 接続点データはなし

	DirectX::XMFLOAT4 floorColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	DirectX::XMFLOAT4 wallColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 pillarColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 床
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		floorColor));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));

	// 壁
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ -2.0f, 0.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ -2.0f, 3.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 3.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ 2.0f, 0.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ 2.0f, 3.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 3.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 0.0f, 2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 3.0f, 2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 0.0f, 2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 3.0f, 2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));

	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ -6.0f, 0.0f, 2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ -6.0f, 3.0f, 2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ 6.0f, 0.0f, 2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
		{ 6.0f, 3.0f, 2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));

	for (int i = 0; i < 3; i++)
	{
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ -6.0f, 0.0f, 2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ -6.0f, 3.0f, 2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
			{ -6.0f, 0.0f, 2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
			{ -6.0f, 3.0f, 2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ 6.0f, 0.0f, 2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ 6.0f, 3.0f, 2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
			{ 6.0f, 0.0f, 2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
			{ 6.0f, 3.0f, 2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
	}
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -6.0f, 0.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -6.0f, 3.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 6.0f, 0.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 6.0f, 3.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 0.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ -2.0f, 3.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 0.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::PILLAR,
		{ 2.0f, 3.0f, 14.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));

	for (int i = 0; i < 3; i++)
	{
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ -6.0f + (4.0f * i), 0.0f, 14.0f },
			{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.emplace_back(TILE_DATA(TileType::WALL,
			{ -6.0f + (4.0f * i), 3.0f, 14.0f },
			{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
	}
}

//void EndRoom1::PlaceMapTile()
//{
//	for (const TILE_DATA& tileData : m_tileDatas)
//	{
//		std::string fileName;
//
//		switch (tileData.type)
//		{
//		case TileType::FLOOR:
//			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
//			break;
//		case TileType::WALL:
//			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
//			continue;
//			break;
//		default:
//			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
//			break;
//		}
//
//		MapTile* newTile = new MapTile(fileName.c_str(), 1.0f, this);
//		newTile->SetPosition(tileData.position);
//		newTile->SetAngle(tileData.angle);
//		newTile->SetScale(tileData.scale);
//		newTile->SetColor(tileData.color);
//		newTile->Update(0);
//		MAPTILES.Register(newTile);
//	}
//}
//
//void EndRoom1::PlaceExit()
//{
//	MapTile* exit = new MapTile("Data/Model/Cube/testCubes.glb", 1.0f, this);
//	exit->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 8.0f));
//	MAPTILES.Register(exit);
//}