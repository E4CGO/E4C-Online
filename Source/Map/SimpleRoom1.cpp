#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "Map/SimpleRoom1.h"

#include "Map/EndRoom1.h"
#include "Map/CrossRoom1.h"
#include "Map/Passage1.h"

// コンストラクタ（乱数による自動生成）
SimpleRoom1::SimpleRoom1(RoomBase* parent, int pointIndex, std::vector<AABB>& roomAABBs)
{
	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	if (this->parent != nullptr)
	{
		this->m_position = parent->GetConnectPointData(pointIndex).position;
		this->m_angle = parent->GetConnectPointData(pointIndex).angle;
	}

	// 接続点データ設定のために行列更新処理を行う
	UpdateTransform();

	// 深度を取得
	depth = GetDepth();

	// 部屋タイプを設定
	roomType = DungeonData::SIMPLE_ROOM_1;

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

	// 部屋データのロード
	LoadMapTileData();

	// AABBをゆるめるぜ
	m_aabb.radii.x -= 2.0f;
	m_aabb.radii.z -= 2.0f;

	// 自身のAABBを配列に入れる
	roomAABBs.emplace_back(m_aabb);

	// 一定の深度まではランダムな部屋を生成する
	// ここから次の部屋の生成
	if (!(depth > DungeonData::Instance().GetDungeonGenerateSetting().maxDepth))
	{
		// 配置時に他の部屋と重ならない部屋のみを配列に保存する
		std::vector<std::vector<DungeonData::RoomType>> placeableRooms;
		placeableRooms.resize(m_connectPointDatas.size());

		// 接続点の数だけ当たり判定を行う
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			for (DungeonData::RoomType type : DungeonData::Instance().GetRoomGenerateSetting(roomType).placementCandidates)
			{
				AABB nextRoomAABB = DungeonData::Instance().GetRoomGenerateSetting(roomType).aabb;

				// 接続点の角度によりAABBを変形させる
				DirectX::XMFLOAT3 connectPointAngle = m_connectPointDatas.at(i).angle;
				
				// 360度以内に丸める
				while (DirectX::XMConvertToRadians(connectPointAngle.y) >= 360.0f) connectPointAngle.y -= DirectX::XMConvertToDegrees(360.0f);
				while (DirectX::XMConvertToRadians(connectPointAngle.y) < 0.0f) connectPointAngle.y += DirectX::XMConvertToDegrees(360.0f);

				// 90度か270度ならxとzを逆転させる
				if ((DirectX::XMConvertToRadians(connectPointAngle.y) > 89.9f && DirectX::XMConvertToRadians(connectPointAngle.y) < 90.1f) ||
					(DirectX::XMConvertToRadians(connectPointAngle.y) > 269.9f && DirectX::XMConvertToRadians(connectPointAngle.y) < 270.1f))
				{
					AABB buf = nextRoomAABB;

					nextRoomAABB.radii.x = nextRoomAABB.radii.z;
					nextRoomAABB.radii.z = buf.radii.x;
				}

				// AABBの中心を移動させる
				nextRoomAABB.position += m_connectPointDatas.at(i).position;

				bool isHit = false;

				// 自分以外のAABBとの当たり判定を行う
				for (const AABB& anotherRoomAABB : roomAABBs)
				{
					IntersectionResult result;

					if (Collision::IntersectAABBVsAABB(
						DirectX::XMLoadFloat3(&nextRoomAABB.position),
						DirectX::XMLoadFloat3(&nextRoomAABB.radii),
						DirectX::XMLoadFloat3(&anotherRoomAABB.position),
						DirectX::XMLoadFloat3(&anotherRoomAABB.radii),
						&result))
					{
						// 自分以外のAABBと衝突するなら配列に保存しない
						isHit = true;
						break;
					}
				}

				// 衝突しなかった場合は配列に保存する
				if (!isHit)
				{
					placeableRooms.at(i).emplace_back(type);
				}
			}
		}

		// 接続点の数だけ子を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			// 他の部屋と重ならない部屋が存在しない場合は処理を行わない
			if (placeableRooms.at(i).size() < 1) continue;

			// 接続可能な部屋の重みの合計
			int totalWeight = 0;
			for (DungeonData::RoomType type : placeableRooms.at(i))
			{
				totalWeight += DungeonData::Instance().GetRoomGenerateSetting(type).weight;
			}

			int randomValue = std::rand() % totalWeight;
			for (DungeonData::RoomType type : placeableRooms.at(i))
			{
				randomValue -= DungeonData::Instance().GetRoomGenerateSetting(type).weight;

				if (randomValue < 0)
				{
					RoomBase* nextRoom = nullptr;

					switch (type)
					{
					case DungeonData::SIMPLE_ROOM_1:
						nextRoom = new SimpleRoom1(this, i, roomAABBs);
						break;

					case DungeonData::END_ROOM:
						nextRoom = new EndRoom1(this, i, roomAABBs);
						break;

					case DungeonData::CROSS_ROOM_1:
						nextRoom = new CrossRoom1(this, i, roomAABBs);
						break;

					case DungeonData::PASSAGE_1:
						nextRoom = new Passage1(this, i, roomAABBs);
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
			RoomBase* nextEndRoom = new EndRoom1(this, i, roomAABBs);
			AddRoom(nextEndRoom);
		}
	}
}

// コンストラクタ（配列に基づいた生成）
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

	// 部屋データをロード
	LoadMapTileData();

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

void SimpleRoom1::LoadMapTileData()
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

	// XとZの最小値、最大値を保存しておき
	// それを元に当たり判定を作成する
	float minX = INT_MAX;
	float maxX = INT_MIN;
	float minZ = INT_MAX;
	float maxZ = INT_MIN;

	for (const TILE_DATA& tileData : m_tileDatas)
	{
		// XとZの最小値、最大値を保存しておく
		if (tileData.position.x < minX) minX = tileData.position.x;
		if (tileData.position.x > maxX) maxX = tileData.position.x;
		if (tileData.position.z < minZ) minZ = tileData.position.z;
		if (tileData.position.z > maxZ) maxZ = tileData.position.z;
	}

	// 行列更新を行うためUpdate呼び出し
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

void SimpleRoom1::PlaceMapTile()
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
			fileName = "Data/Model/Dungeon assets/SM_Wall_01a.fbx";
			break;
		default:
			continue;
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