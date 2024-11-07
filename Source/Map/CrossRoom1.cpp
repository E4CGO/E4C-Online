﻿#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "Map/CrossRoom1.h"

#include "Map/SimpleRoom1.h"
#include "Map/EndRoom1.h"
#include "Map/Passage1.h"

CrossRoom1::CrossRoom1(RoomBase* parent, int pointIndex, std::vector<int> roomTree, int& treeIndex)
{
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	depth = GetDepth();

	roomType = DungeonData::CROSS_ROOM_1;


	// 接続点データ
	CONNECTPOINT_DATA point1;
	point1.position = { 12.0f, 0.0f, 12.0f };
	point1.angle = { 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point1);

	CONNECTPOINT_DATA point2;
	point2.position = { -12.0f, 0.0f, 12.0f };
	point2.angle = { 0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point2);

	CONNECTPOINT_DATA point3;
	point3.position = { 0.0f, 0.0f, 24.0f };
	point3.angle = { 0.0f, 0.0f, 0.0f };
	m_connectPointDatas.emplace_back(point3);


	// 接続点の数だけ子を生成する
	for (int i = 0; i < m_connectPointDatas.size(); i++)
	{
		RoomBase* nextRoom = nullptr;

		if (treeIndex >= roomTree.size()) break;

		switch (roomTree[treeIndex])
		{
		case DungeonData::SIMPLE_ROOM_1:
			nextRoom = new SimpleRoom1(this, i, roomTree, ++treeIndex);
			break;

		case DungeonData::END_ROOM:
			nextRoom = new EndRoom1(this, i, roomTree, ++treeIndex);
			break;

		case DungeonData::CROSS_ROOM_1:
			nextRoom = new CrossRoom1(this, i, roomTree, ++treeIndex);
			break;

		case DungeonData::PASSAGE_1:
			nextRoom = new Passage1(this, i, roomTree, ++treeIndex);
			break;
		}
		AddRoom(nextRoom);
	}
}

CrossRoom1::CrossRoom1(RoomBase* parent, int pointIndex)
{
	//this->parent = parent;
	//this->parentConnectPointIndex = pointIndex;

	//depth = GetDepth();

	//roomType = DungeonData::CROSS_ROOM_1;


	//// 接続点データ
	//CONNECTPOINT_DATA point1;
	//point1.position = { 12.0f, 0.0f, 12.0f };
	//point1.angle = { 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f };
	//m_connectPointDatas.emplace_back(point1);

	//CONNECTPOINT_DATA point2;
	//point2.position = { -12.0f, 0.0f, 12.0f };
	//point2.angle = { 0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f };
	//m_connectPointDatas.emplace_back(point2);

	//CONNECTPOINT_DATA point3;
	//point3.position = { 0.0f, 0.0f, 24.0f };
	//point3.angle = { 0.0f, 0.0f, 0.0f };
	//m_connectPointDatas.emplace_back(point3);


	//// 一定の深度まではランダムな部屋を生成する
	//if (!(depth > DungeonData::Instance().GetDungeonGenerateSetting().maxDepth))
	//{
	//	// 接続可能な部屋を設定
	//	m_connectableRooms.emplace_back(DungeonData::SIMPLE_ROOM_1);
	//	m_connectableRooms.emplace_back(DungeonData::END_ROOM);
	//	//m_connectableRooms.emplace_back(DungeonData::CROSS_ROOM_1);
	//	m_connectableRooms.emplace_back(DungeonData::PASSAGE_1);

	//	// 接続可能な部屋の重みの合計
	//	int totalWeight = 0;
	//	for (DungeonData::RoomType type : m_connectableRooms)
	//	{
	//		totalWeight += DungeonData::Instance().GetRoomGenerateSetting(type).weight;
	//	}

	//	// 接続点の数だけ子を生成する
	//	for (int i = 0; i < m_connectPointDatas.size(); i++)
	//	{
	//		int randomValue = std::rand() % totalWeight;

	//		for (DungeonData::RoomType type : m_connectableRooms)
	//		{
	//			randomValue -= DungeonData::Instance().GetRoomGenerateSetting(type).weight;

	//			if (randomValue < 0)
	//			{
	//				RoomBase* nextRoom = nullptr;

	//				switch (type)
	//				{
	//				case DungeonData::SIMPLE_ROOM_1:
	//					nextRoom = new SimpleRoom1(this, i);
	//					break;

	//				case DungeonData::END_ROOM:
	//					nextRoom = new EndRoom1(this, i);
	//					break;

	//				case DungeonData::CROSS_ROOM_1:
	//					nextRoom = new CrossRoom1(this, i);
	//					break;

	//				case DungeonData::PASSAGE_1:
	//					nextRoom = new Passage1(this, i);
	//					break;

	//				default:
	//					break;
	//				}
	//				AddRoom(nextRoom);
	//				break;
	//			}
	//		}
	//	}
	//}
	//// 一定の深度を超えた場合は終端の部屋を生成する
	//else
	//{
	//	// 接続点の数だけ終端の部屋を生成する
	//	for (int i = 0; i < m_connectPointDatas.size(); i++)
	//	{
	//		RoomBase* nextEndRoom = new EndRoom1(this, i);
	//		AddRoom(nextEndRoom);
	//	}
	//}
}

void CrossRoom1::LoadMapTileData()
{
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR, { 0.0f, 0.0f, 0.0f }));
	m_tileDatas.emplace_back(TILE_DATA(TileType::WALL, { 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f)));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(0.0f, 0.0f, 20.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(4.0f, 0.0f, 20.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(8.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(8.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(8.0f, 0.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-4.0f, 0.0f, 20.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-8.0f, 0.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-8.0f, 0.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
		DirectX::XMFLOAT3(-8.0f, 0.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));

	float minX = INT_MAX;
	float maxX = INT_MIN;
	float minZ = INT_MAX;
	float maxZ = INT_MIN;

	for (const TILE_DATA& tileData : m_tileDatas)
	{
		if (tileData.position.x < minX) minX = tileData.position.x;
		if (tileData.position.x > maxX) maxX = tileData.position.x;
		if (tileData.position.z < minZ) minZ = tileData.position.z;
		if (tileData.position.z > maxZ) maxZ = tileData.position.z;
	}

	Update(0);

	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);

	// AABB
	m_aabb.position = {
		(minX + maxX) * 0.5f,
		1.0f,
		(minZ + maxZ) * 0.5f
	};

	// ワールド座標に変換し保存
	DirectX::XMVECTOR AABBPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_aabb.position), WorldTransform);
	DirectX::XMStoreFloat3(&m_aabb.position, AABBPos);

	m_aabb.radii.x = (maxX - minX);
	m_aabb.radii.y = 1.0f;
	m_aabb.radii.z = (maxZ - minZ);
}

void CrossRoom1::PlaceMapTile()
{
	for (const TILE_DATA& tileData : m_tileDatas)
	{
		std::string fileName;

		switch (tileData.type)
		{
		case TileType::FLOOR:
			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
			break;
		case TileType::WALL:
			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
			continue;
			break;
		default:
			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
			break;
		}

		MapTile* newTile = new MapTile(fileName.c_str(), 1.0f, this);
		newTile->SetPosition(tileData.position);
		newTile->SetAngle(tileData.angle);
		newTile->SetScale(tileData.scale);
		newTile->SetColor(tileData.color);
		newTile->Update(0);
		MAPTILES.Register(newTile);
	}
}