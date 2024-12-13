#include "Map/SimpleRoom1.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/Network/WinSock2Wrapper.h"

#include <fstream>

SimpleRoom1::SimpleRoom1(
	RoomBase* parent, int pointIndex,
	std::vector<AABB>& roomAABBs,
	bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
	: RoomBase(parent, pointIndex, roomAABBs, isAutoGeneration, roomOrder, orderIndex)
{
	// 部屋タイプを設定
	roomType = RoomType::SIMPLE_ROOM_1;

	// 次の部屋の生成を行う
	GenerateNextRoom(
		roomAABBs,
		isAutoGeneration,
		roomOrder, orderIndex);
}

void SimpleRoom1::LoadMapData()
{
	// 接続点データを設定
	{
		TILE_DATA newPoint;
		newPoint.position = DirectX::XMFLOAT3(12.0f, 0.0f, 8.0f);
		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f);

		// ワールド座標に変換し保存
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
		m_connectPointDatas.emplace_back(newPoint);
	}

	{
		TILE_DATA newPoint;
		newPoint.position = DirectX::XMFLOAT3(-12.0f, 0.0f, 8.0f);
		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f);

		// ワールド座標に変換し保存
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
		m_connectPointDatas.emplace_back(newPoint);
	}

	DirectX::XMFLOAT4 floorColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	DirectX::XMFLOAT4 wallColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 pillarColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	{ 4.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	{ 8.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	{ -4.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	{ -8.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));



	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 0.0f, 4.0f, 0.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f },
	//	floorColor));

	nlohmann::json loadFile;
	std::ifstream ifs("Data/RoomDatas/SimpleRoom1.json");

	if (ifs.is_open())
	{
		ifs >> loadFile;

		// ノードデータロード
		for (const auto& nodeData : loadFile["NodeDatas"])
		{
			TileType tileType = nodeData["Type"];
			DirectX::XMFLOAT3 position = {
				nodeData["Position"].at(0),
				nodeData["Position"].at(1),
				nodeData["Position"].at(2)
			};
			DirectX::XMFLOAT3 angle = {
				nodeData["Angle"].at(0),
				nodeData["Angle"].at(1),
				nodeData["Angle"].at(2)
			};
			DirectX::XMFLOAT3 scale = {
				nodeData["Scale"].at(0),
				nodeData["Scale"].at(1),
				nodeData["Scale"].at(2),
			};

			m_tileDatas.at(tileType).emplace_back(TILE_DATA(
				position,
				angle,
				scale));
		}
		ifs.close();
	}

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -2.0f, 0.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -2.0f, 3.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 2.0f, 0.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 2.0f, 3.0f, -2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));



	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -6.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -6.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 2.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 2.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 6.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 6.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));


	//
	//for (int i = -3; i < 2; i++)
	//{
	//	m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//		{ (4.0f * i) + 2.0f, 0.0f, 14.0f},
	//		{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//		{ 1.0f, 1.0f, 1.0f }));
	//	m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//		{ (4.0f * i) + 2.0f, 3.0f, 14.0f },
	//		{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
	//		{ 1.0f, 1.0f, 1.0f }));
	//}



	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	////m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	////	{ -10.0f, 0.0f, 6.0f },
	////	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	////	{ -10.0f, 3.0f, 6.0f },
	////	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ -10.0f, 0.0f, 6.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ -10.0f, 3.0f, 6.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 0.0f, 10.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ -10.0f, 3.0f, 10.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ -10.0f, 0.0f, 10.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ -10.0f, 3.0f, 10.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));



	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 10.0f, 0.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 10.0f, 3.0f, 2.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));

	////m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	////	{ -10.0f, 0.0f, 6.0f },
	////	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	////	{ -10.0f, 3.0f, 6.0f },
	////	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ 10.0f, 0.0f, 6.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ 10.0f, 3.0f, 6.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));

	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 10.0f, 0.0f, 10.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	//m_tileDatas.at(TileType::WALL_01A).emplace_back(TILE_DATA(
	//	{ 10.0f, 3.0f, 10.0f },
	//	{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
	//	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ 10.0f, 0.0f, 10.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));
	////m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
	////	{ 10.0f, 3.0f, 10.0f },
	////	{ 0.0f, 0.0f, 0.0f },
	////	{ 1.0f, 1.0f, 1.0f }));



	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 4.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 8.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 12.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(4.0f, 0.0f, 4.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(4.0f, 0.0f, 8.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(4.0f, 0.0f, 12.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(8.0f, 0.0f, 4.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(8.0f, 0.0f, 8.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(8.0f, 0.0f, 12.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-4.0f, 0.0f, 4.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-4.0f, 0.0f, 8.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-4.0f, 0.0f, 12.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));

	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-8.0f, 0.0f, 4.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-8.0f, 0.0f, 8.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
	//m_tileDatas.at(TileType::FLOOR_01A).emplace_back(TILE_DATA(
	//	DirectX::XMFLOAT3(-8.0f, 0.0f, 12.0f),
	//	DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
	//	floorColor));
}