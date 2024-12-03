#include "Passage1.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

Passage1::Passage1(
	RoomBase* parent, const int pointIndex,
	std::vector<AABB>& roomAABBs,
	const bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
	: RoomBase(parent, pointIndex, roomAABBs, isAutoGeneration, roomOrder, orderIndex)
{
	// 部屋タイプを設定
	roomType = RoomType::PASSAGE_1;

	// 次の部屋の生成を行う
	GenerateNextRoom(
		roomAABBs,
		isAutoGeneration,
		roomOrder, orderIndex);
}

void Passage1::LoadMapData()
{
	// 接続点データを設定
	{
		CONNECTPOINT_DATA newPoint;
		newPoint.position = DirectX::XMFLOAT3(0.0f, 3.0f, 24.0f);
		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		// ワールド座標に変換し保存
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
		m_connectPointDatas.emplace_back(newPoint);
	}

	DirectX::XMFLOAT4 floorColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	DirectX::XMFLOAT4 wallColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 pillarColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_tileDatas.at(TileType::STAIR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));

	m_tileDatas.at(TileType::FLOOR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 3.0f, 4.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.at(TileType::FLOOR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 3.0f, 8.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.at(TileType::FLOOR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 3.0f, 12.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.at(TileType::FLOOR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 3.0f, 16.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));
	m_tileDatas.at(TileType::FLOOR).emplace_back(TILE_DATA(
		DirectX::XMFLOAT3(0.0f, 3.0f, 20.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		floorColor));

	// 壁
	m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
		{ -2.0f, 0.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
		{ -2.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
		{ 2.0f, 0.0f, -2.0f },
		{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
		{ 1.0f, 1.0f, 1.0f }));
	m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
		{ 2.0f, 0.0f, -2.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }));

	for (int i = 0; i < 6; i++)
	{
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ -2.0f, 3.0f, -2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ -2.0f, 6.0f, -2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ -2.0f, 3.0f, -2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ -2.0f, 6.0f, -2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ 2.0f, 3.0f, -2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ 2.0f, 6.0f, -2.0f + (4.0f * i) },
			{ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ 2.0f, 3.0f, -2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ 2.0f, 6.0f, -2.0f + (4.0f * i) },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
	}
}