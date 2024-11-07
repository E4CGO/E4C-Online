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

	// 部屋データのロード
	LoadMapTileData();

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
				
				// 360°以内に丸める
				//connectPointAngle = 

				//if (connectPointAngle.y > DirectX::XMConvertToRadians(89.9f) && connectPointAngle.y < DirectX::XMConvertToRadians(91.0f))

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
					}
					else
					{
						//placeableRooms.at(i).emplace_back(type);
					}
					placeableRooms.at(i).emplace_back(type);
					break;
				}
			}
		}

		// 接続点の数だけ子を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
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
						//nextRoom = new EndRoom1(this, i);
						break;

					case DungeonData::CROSS_ROOM_1:
						//nextRoom = new CrossRoom1(this, i);
						break;

					case DungeonData::PASSAGE_1:
						//nextRoom = new Passage1(this, i);
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

void SimpleRoom1::Update(float elapsedTime)
{
	UpdateTransform();

	for (RoomBase* child : childs)
	{
		child->Update(elapsedTime);
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
			//fileName = "Data/Model/Dungeon/Floor_Plain_Parent.glb";
			fileName = "Data/Model/Dungeon/Floor_Plain_Parent_new.glb";
			break;
		case TileType::WALL:
			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
			fileName = "Data/Model/Dungeon/Doorway Parent 006_new.glb";
			//continue;
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
		//mapTiles.emplace_back(newTile);
	}
	// 最後にMapTileManagerに登録する
	//for (MapTile* tile : mapTiles)
	//{
	//	MAPTILES.Register(tile);
	//}
}