﻿#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "Map/SimpleRoom1.h"

#include "Map/EndRoom1.h"
#include "Map/CrossRoom1.h"
#include "Map/Passage1.h"

// コンストラクタ（配列から生成を行う）
SimpleRoom1::SimpleRoom1(RoomBase* parent, int pointIndex, std::vector<int> roomTree, int& treeIndex)
{
	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	// 深度を取得
	depth = GetDepth();

	// 部屋タイプを設定
	roomType = DungeonData::SIMPLE_ROOM_1;


	// 接続点データを設定
	CONNECTPOINT_DATA point1;
	point1.position = { 12.0f, 0.0f, 8.0f };
	point1.angle = { 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point1);

	CONNECTPOINT_DATA point2;
	point2.position = { -12.0f, 0.0f, 8.0f };
	point2.angle = { 0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point2);


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

// コンストラクタ（乱数を用いて生成を行う）
SimpleRoom1::SimpleRoom1(RoomBase* parent, int pointIndex)
{
	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	// 深度を取得
	depth = GetDepth();

	// 部屋タイプを設定
	roomType = DungeonData::SIMPLE_ROOM_1;


	// 接続点データを設定
	CONNECTPOINT_DATA point1;
	point1.position = { 12.0f, 0.0f, 8.0f };
	point1.angle = { 0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point1);

	CONNECTPOINT_DATA point2;
	point2.position = { -12.0f, 0.0f, 8.0f };
	point2.angle = { 0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f };
	m_connectPointDatas.emplace_back(point2);


	// 一定の深度まではランダムな部屋を生成する
	if (!(depth > DungeonData::Instance().GetDungeonGenerateSetting().maxDepth))
	{
		// 接続可能な部屋を設定
		m_connectableRooms.emplace_back(DungeonData::SIMPLE_ROOM_1);
		//m_connectableRooms.emplace_back(DungeonData::END_ROOM);
		m_connectableRooms.emplace_back(DungeonData::CROSS_ROOM_1);
		m_connectableRooms.emplace_back(DungeonData::PASSAGE_1);

		// 接続可能な部屋の重みの合計
		int totalWeight = 0;
		for (DungeonData::RoomType type : m_connectableRooms)
		{
			totalWeight += DungeonData::Instance().GetRoomGenerateSetting(type).weight;
		}

		// 接続点の数だけ子を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			int randomValue = std::rand() % totalWeight;

			for (DungeonData::RoomType type : m_connectableRooms)
			{
				randomValue -= DungeonData::Instance().GetRoomGenerateSetting(type).weight;

				if (randomValue < 0)
				{
					RoomBase* nextRoom = nullptr;

					switch (type)
					{
					case DungeonData::SIMPLE_ROOM_1:
						nextRoom = new SimpleRoom1(this, i);
						break;

					case DungeonData::END_ROOM:
						nextRoom = new EndRoom1(this, i);
						break;

					case DungeonData::CROSS_ROOM_1:
						nextRoom = new CrossRoom1(this, i);
						break;

					case DungeonData::PASSAGE_1:
						nextRoom = new Passage1(this, i);
						break;

					default:
						break;
					}
					AddRoom(nextRoom);
					break;
				}
			}
		}
	}
	// 一定の深度を超えた場合は終端の部屋を生成する
	else
	{
		// 接続点の数だけ終端の部屋を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			RoomBase* nextEndRoom = new EndRoom1(this, i);
			AddRoom(nextEndRoom);
		}
	}
}

void SimpleRoom1::PlaceMapTile()
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
		DirectX::XMFLOAT3(8.0f, 0.0f, 4.0f),
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
		DirectX::XMFLOAT3(-8.0f, 0.0f, 4.0f),
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

	// 接続点
	//m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
	//	(m_startPos + DirectX::XMFLOAT3(12.0f, 0.0f, 8.0f)),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));

	//m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
	//	(m_startPos + DirectX::XMFLOAT3(-12.0f, 0.0f, 8.0f)),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));

	//// XとZの最小値、最大値を保存しておき
	//// それを元に当たり判定を作成する
	//float minX = INT_MAX;
	//float maxX = INT_MIN;
	//float minZ = INT_MAX;
	//float maxZ = INT_MIN;

	for (const TILE_DATA& tileData : m_tileDatas)
	{
		std::string fileName;

		switch (tileData.type)
		{
		case TileType::FLOOR:
			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
			//fileName = "Data/Model/Stage/BigMap.glb";
			break;
		case TileType::WALL:
			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
			continue;
			break;
		default:
			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
			break;
		}



		//// XとZの最小値、最大値を保存しておく
		//if (tileData.position.x < minX) minX = tileData.position.x;
		//if (tileData.position.x > maxX) maxX = tileData.position.x;
		//if (tileData.position.z < minZ) minZ = tileData.position.z;
		//if (tileData.position.z > maxZ) maxZ = tileData.position.z;

		MapTile* newTile = new MapTile(fileName.c_str(), 1.0f, this);
		newTile->SetPosition(tileData.position);
		newTile->SetAngle(tileData.angle);
		newTile->SetScale(tileData.scale);
		newTile->SetColor(tileData.color);
		MAPTILES.Register(newTile);
	}
}

int SimpleRoom1::DrawDebugGUI(int i)
{
	if (ImGui::TreeNode(("SimpleRoom1(" + std::to_string(i) + ")").c_str()))
	{
		DirectX::XMFLOAT3 debugAngle;
		debugAngle.x = DirectX::XMConvertToDegrees(m_angle.x);
		debugAngle.y = DirectX::XMConvertToDegrees(m_angle.y);
		debugAngle.z = DirectX::XMConvertToDegrees(m_angle.z);
		ImGui::DragFloat3("Angle", &debugAngle.x);
		m_angle.x = DirectX::XMConvertToRadians(debugAngle.x);
		m_angle.y = DirectX::XMConvertToRadians(debugAngle.y);
		m_angle.z = DirectX::XMConvertToRadians(debugAngle.z);

		if (ImGui::TreeNode("ConnectPoints"))
		{
			for (int j = 0; j < m_connectPointDatas.size(); j++)
			{
				if (ImGui::TreeNode(("Point(" + std::to_string(j) + ")").c_str()))
				{
					ImGui::PushID(j);
					ImGui::DragFloat3("Position", &m_connectPointDatas[j].position.x);
					DirectX::XMFLOAT3 debugConnectAngle;
					debugConnectAngle.x = DirectX::XMConvertToDegrees(m_connectPointDatas[j].angle.x);
					debugConnectAngle.y = DirectX::XMConvertToDegrees(m_connectPointDatas[j].angle.y);
					debugConnectAngle.z = DirectX::XMConvertToDegrees(m_connectPointDatas[j].angle.z);
					ImGui::DragFloat3("Angle", &debugConnectAngle.x);
					m_connectPointDatas[j].angle.x = DirectX::XMConvertToRadians(debugConnectAngle.x);
					m_connectPointDatas[j].angle.y = DirectX::XMConvertToRadians(debugConnectAngle.y);
					m_connectPointDatas[j].angle.z = DirectX::XMConvertToRadians(debugConnectAngle.z);
					ImGui::PopID();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	for (RoomBase* child : childs)
	{
		i = child->DrawDebugGUI(++i);
	}
	return i;
}