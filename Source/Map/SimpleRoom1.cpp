#include "Map/SimpleRoom1.h"

#include <DirectXMath.h>
#include "TAKOEngine/Tool/XMFLOAT.h"

SimpleRoom1::SimpleRoom1(
	RoomBase* parent,
	int pointIndex) : RoomBase(parent, pointIndex)
{
	// 部屋タイプを設定
	roomType = DungeonData::SIMPLE_ROOM_1;

	// 次の部屋の生成を行う
	GenerateNextRoom();
}

void SimpleRoom1::LoadMapData()
{
	// 接続点データを設定
	{
		CONNECTPOINT_DATA newPoint;
		newPoint.position = DirectX::XMFLOAT3(12.0f, 0.0f, 8.0f);
		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f);

		// ワールド座標に変換し保存
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
		m_connectPointDatas.emplace_back(newPoint);
	}

	{
		CONNECTPOINT_DATA newPoint;
		newPoint.position = DirectX::XMFLOAT3(-12.0f, 0.0f, 8.0f);
		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(-90.0f), 0.0f);

		// ワールド座標に変換し保存
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
		m_connectPointDatas.emplace_back(newPoint);
	}

	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR, { 0.0f, 0.0f, 0.0f }));
	//m_tileDatas.emplace_back(TILE_DATA(TileType::WALL, { 0.0f, 0.0f, 0.0f },
	//	DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	//	{ 1.0f, 1.0f, 1.0f }));

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

	//// XとZの最小値、最大値を保存しておき
	//// それを元に当たり判定を作成する
	//float minX = FLT_MAX;
	//float maxX = FLT_MIN;
	//float minZ = FLT_MAX;
	//float maxZ = FLT_MIN;

	//for (const TILE_DATA& tileData : m_tileDatas)
	//{
	//	// XとZの最小値、最大値を保存しておく
	//	if (tileData.position.x < minX) minX = tileData.position.x;
	//	if (tileData.position.x > maxX) maxX = tileData.position.x;
	//	if (tileData.position.z < minZ) minZ = tileData.position.z;
	//	if (tileData.position.z > maxZ) maxZ = tileData.position.z;
	//}

	//maxX += 4.0f;
	//maxZ += 4.0f;

	////// 行列更新を行うためUpdate呼び出し
	////Update(0);

	//DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);

	//// AABB
	//m_aabb.position = {
	//	(minX + maxX) * 0.5f,
	//	1.0f,
	//	(minZ + maxZ) * 0.5f
	//};

	//// ワールド座標に変換し保存
	//DirectX::XMVECTOR AABBPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_aabb.position), WorldTransform);
	//DirectX::XMStoreFloat3(&m_aabb.position, AABBPos);

	//m_aabb.radii.x = (maxX - minX) * 0.5f;
	//m_aabb.radii.y = 1.0f;
	//m_aabb.radii.z = (maxZ - minZ) * 0.5f;

	//// 360度以内に丸める
	//while (DirectX::XMConvertToDegrees(m_angle.y) >= 360.0f) m_angle.y -= DirectX::XMConvertToRadians(360.0f);
	//while (DirectX::XMConvertToDegrees(m_angle.y) < 0.0f) m_angle.y += DirectX::XMConvertToRadians(360.0f);

	//// 角度によって位置補正を行う
	//// 90度
	//if (DirectX::XMConvertToDegrees(m_angle.y) > 89.9f && DirectX::XMConvertToDegrees(m_angle.y) < 90.1f)
	//{
	//	m_aabb.position.z += tileScale;
	//}

	//// 180度
	//if (DirectX::XMConvertToDegrees(m_angle.y) > 179.9f && DirectX::XMConvertToDegrees(m_angle.y) < 180.1f)
	//{
	//	m_aabb.position.x += tileScale;
	//	m_aabb.position.z += tileScale;
	//}

	//// 270度
	//if (DirectX::XMConvertToDegrees(m_angle.y) > 269.9f && DirectX::XMConvertToDegrees(m_angle.y) < 270.1f)
	//{
	//	m_aabb.position.x -= tileScale;
	//}

	//// 90度か270度ならxとzを逆転させる
	//if ((DirectX::XMConvertToDegrees(m_angle.y) > 89.9f && DirectX::XMConvertToDegrees(m_angle.y) < 90.1f) ||
	//	(DirectX::XMConvertToDegrees(m_angle.y) > 269.9f && DirectX::XMConvertToDegrees(m_angle.y) < 270.1f))
	//{
	//	AABB buf = m_aabb;

	//	m_aabb.position.x = m_aabb.position.z;
	//	m_aabb.position.z = buf.position.x;

	//	m_aabb.radii.x = m_aabb.radii.z;
	//	m_aabb.radii.z = buf.radii.x;
	//}
}