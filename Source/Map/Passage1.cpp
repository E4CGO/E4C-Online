//#include "Map/MapTileManager.h"
//#include "Map/MapTile.h"
//
//#include "Passage1.h"
//
//#include "Map/SimpleRoom1.h"
//#include "Map/EndRoom1.h"
//#include "Map/CrossRoom1.h"
//
//void Passage1::LoadMapData()
//{
//	// 部屋タイプを設定
//	roomType = DungeonData::PASSAGE_1;
//
//	// 接続点データを設定
//	{
//		CONNECTPOINT_DATA newPoint;
//		newPoint.position = DirectX::XMFLOAT3(0.0f, 3.0f, 24.0f);
//		newPoint.angle = m_angle + DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//
//		// ワールド座標に変換し保存
//		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
//		DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
//		DirectX::XMStoreFloat3(&newPoint.position, PointPos);
//		m_connectPointDatas.emplace_back(newPoint);
//	}
//
//	m_tileDatas.emplace_back(TILE_DATA(TileType::STAIR,
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f)));
//
//	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
//		DirectX::XMFLOAT3(0.0f, 3.0f, 4.0f),
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
//		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
//	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
//		DirectX::XMFLOAT3(0.0f, 3.0f, 8.0f),
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
//		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
//	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
//		DirectX::XMFLOAT3(0.0f, 3.0f, 12.0f),
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
//		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
//	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
//		DirectX::XMFLOAT3(0.0f, 3.0f, 16.0f),
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
//		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
//	m_tileDatas.emplace_back(TILE_DATA(TileType::FLOOR,
//		DirectX::XMFLOAT3(0.0f, 3.0f, 20.0f),
//		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
//		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
//		DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)));
//}
//
////void Passage1::PlaceMapTile()
////{
////	for (const TILE_DATA& tileData : m_tileDatas)
////	{
////		std::string fileName;
////
////		switch (tileData.type)
////		{
////		case TileType::FLOOR:
////			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
////			break;
////		case TileType::WALL:
////			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
////			continue;
////			break;
////		case TileType::STAIR:
////			fileName = "Data/Model/Dungeon/Stair Parent 001.glb";
////			break;
////		default:
////			fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
////			break;
////		}
////
////		MapTile* newTile = new MapTile(fileName.c_str(), 1.0f, this);
////		newTile->SetPosition(tileData.position);
////		newTile->SetAngle(tileData.angle);
////		newTile->SetScale(tileData.scale);
////		newTile->SetColor(tileData.color);
////		newTile->Update(0);
////		MAPTILES.Register(newTile);
////	}
////}