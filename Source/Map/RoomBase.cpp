#include "Source/Map/RoomBase.h"

#include <TAKOEngine\Tool\XMFLOAT.h>
#include <filesystem>
#include <imgui.h>

#include "GameObject/GameObjectManager.h"
#include "GameObject/Props/Spawner.h"
#include "GameObject/Props/SpawnerManager.h"
#include "GameObject/Props/StairToNextFloor.h"
#include "MapTile.h"
#include "MapTileManager.h"

RoomBase::RoomBase(
	RoomBase* parent, int pointIndex,
	RoomType roomType,
	std::vector<AABB>& roomAABBs,
	bool& isLastRoomGenerated)
{
	this->roomType = roomType;

	// タイルデータのリサイズ
	m_tileDatas.resize(TileType::TILETYPE_COUNT);

	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	// 親の接続点データから座標の補正を行う
	if (this->parent != nullptr)
	{
		this->m_position = parent->GetConnectPointData(pointIndex).position;
		this->m_angle = parent->GetConnectPointData(pointIndex).angle;
	}

	// AABB描画用
	m_aabbCube = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
	//m_debugCubes.resize(64);
	for (std::unique_ptr<CubeRenderer>& cubeRenderer : m_debugCubes)
	{
		cubeRenderer = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
	}

	// 深度を取得
	depth = GetDepth();

	// 後の接続点データ設定、部屋モデル配置のために行列更新処理を行う
	UpdateTransform();

	// 部屋データのロード
	LoadMapData();

	// 自身のAABBを算出
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	if (m_connectPointDatas.size() > 0)
	{
		// 次の部屋の生成を行う
		GenerateNextRoomAutomatically(roomAABBs, isLastRoomGenerated);
	}
	else
	{
		// 自身のAABBを配列に登録する
		roomAABBs.emplace_back(m_aabb);
	}
}

RoomBase::RoomBase(
	RoomBase* parent, int pointIndex,
	RoomType roomType,
	std::vector<AABB>& roomAABBs,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
{
	this->roomType = roomType;

	// タイルデータのリサイズ
	m_tileDatas.resize(TileType::TILETYPE_COUNT);

	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	// 親の接続点データから座標の補正を行う
	if (this->parent != nullptr)
	{
		this->m_position = parent->GetConnectPointData(pointIndex).position;
		this->m_angle = parent->GetConnectPointData(pointIndex).angle;
	}

	// AABB描画用
	m_aabbCube = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
	m_debugCubes.resize(16);
	for (std::unique_ptr<CubeRenderer>& cubeRenderer : m_debugCubes)
	{
		cubeRenderer = std::make_unique<CubeRenderer>(T_GRAPHICS.GetDeviceDX12());
	}

	// 深度を取得
	depth = GetDepth();

	// 後の接続点データ設定、部屋モデル配置のために行列更新処理を行う
	UpdateTransform();

	// 部屋データのロード
	LoadMapData();

	// 自身のAABBを算出
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// 次の部屋の生成を行う
	GenerateNextRoomFromOrder(roomAABBs, roomOrder, orderIndex);
}

RoomBase::RoomBase(
	RoomBase* parent, int pointIndex,
	RoomType roomType)
{
	this->roomType = roomType;

	// タイルデータのリサイズ
	m_tileDatas.resize(TileType::TILETYPE_COUNT);

	// 親と接続点番号を代入
	this->parent = parent;
	this->parentConnectPointIndex = pointIndex;

	// 親の接続点データから座標の補正を行う
	if (this->parent != nullptr)
	{
		this->m_position = parent->GetConnectPointData(pointIndex).position;
		this->m_angle = parent->GetConnectPointData(pointIndex).angle;
	}

	// 当たり判定として使えるように行列更新処理を行う
	UpdateTransform();

	// 部屋データのロード
	LoadMapData();

	// 自身のAABBを算出
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// 部屋の生成は行わない
}

void RoomBase::UpdateTransform()
{
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		DirectX::XMMATRIX R = AnglesToMatrix(m_angle);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		DirectX::XMMATRIX LocalTransform = S * R * T;

		DirectX::XMStoreFloat4x4(&m_transform, LocalTransform);
	}
}

void RoomBase::GenerateNextRoomAutomatically(
	std::vector<AABB>& roomAABBs,
	bool& isLastRoomGenerated)
{
	//// 自身のAABBを算出
	//m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenSetting(roomType).aabb,
	//	m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// 最後の部屋が既に生成されているなら通常の生成を行う
	if (isLastRoomGenerated)
	{
		// 最大深度が設定値より浅いなら次の部屋を生成する
		if (depth < DUNGEONDATA.GetCurrentFloorGenSetting().maxDepth)
		{
			// 配置時に他の部屋と重ならない部屋のみを配列に保存する
			std::vector<std::vector<RoomType>> placeableRooms;
			placeableRooms.resize(m_connectPointDatas.size());

			// 接続点の数だけ当たり判定を行い、生成を行う
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				for (RoomType type : DUNGEONDATA.GetRoomGenSetting(roomType).placementCandidates)
				{
					// 新規生成する子のAABBを算出
					AABB nextAABB = CalcAABB(DUNGEONDATA.GetRoomGenSetting(type).aabb,
						m_connectPointDatas.at(i).position, DirectX::XMConvertToDegrees(m_connectPointDatas.at(i).angle.y));

					// 新規生成する子のAABBと他のAABB（この部屋のAABBは除く）との衝突判定を行う
					// 子のAABBはちょっと大きくして部屋が干渉するのを防ぐ
					bool isHit = false;
					for (const AABB& anotherAABB : roomAABBs)
					{
						// AABBの位置が同じ==自分のAABBと判定して処理をスキップする
						if (Mathf::cmpf(anotherAABB.position.x, m_aabb.position.x) &&
							Mathf::cmpf(anotherAABB.position.y, m_aabb.position.y) &&
							Mathf::cmpf(anotherAABB.position.z, m_aabb.position.z)) continue;

						IntersectionResult result;
						DirectX::XMFLOAT3 tmpNextRadii = {
							nextAABB.radii.x + 4.0f,
							nextAABB.radii.y,
							nextAABB.radii.z + 4.0f
						};
						DirectX::XMFLOAT3 tmpAnotherRadii = anotherAABB.radii;

						if (Collision::IntersectAABBVsAABB(
							DirectX::XMLoadFloat3(&nextAABB.position),
							DirectX::XMLoadFloat3(&tmpNextRadii),
							DirectX::XMLoadFloat3(&anotherAABB.position),
							DirectX::XMLoadFloat3(&tmpAnotherRadii),
							&result))
						{
							// AABBと衝突するなら配列に保存しない
							isHit = true;

							// 当たり判定に使用したAABBはデバッグ四角形描画のために保存しておく
							//m_debugAABBs.emplace_back(AABB(nextAABB.position, tmpNextRadii));

							break;
						}
					}
					// 衝突しなかった場合は配列に保存する
					if (!isHit)
					{
						placeableRooms.at(i).emplace_back(type);
					}
				}

				// 当たり判定が終わったところで
				// 自身のAABBを配列に保存
				roomAABBs.emplace_back(m_aabb);

				// 子の部屋を生成する
				// 他の部屋と重ならない部屋があるならば生成を開始する
				if (placeableRooms.at(i).size() > 0)
				{
					// 生成可能な部屋の重みの合計
					int totalWeight = 0;
					for (RoomType type : placeableRooms.at(i))
					{
						totalWeight += DUNGEONDATA.GetRoomGenSetting(type).weight;
					}

					int randomValue = std::rand() % totalWeight;
					for (RoomType type : placeableRooms.at(i))
					{
						randomValue -= DUNGEONDATA.GetRoomGenSetting(type).weight;

						if (randomValue < 0)
						{
							RoomBase* nextRoom = new RoomBase(this, i, type, roomAABBs, isLastRoomGenerated);
							AddRoom(nextRoom);
							break;
						}
					}
				}
				// 何も生成できないならば行き止まり用の部屋を生成する
				else
				{
					RoomBase* deadEnd = new RoomBase(this, i, DUNGEONDATA.GetCurrentFloorGenSetting().deadEndRoomType, roomAABBs, isLastRoomGenerated);
					AddRoom(deadEnd);
				}
			}
		}
		// 一定の深度を超えた場合は行き止まり用の部屋を生成する
		else
		{
			// 自身のAABBを配列に保存
			roomAABBs.emplace_back(m_aabb);

			// 接続点の数だけ行き止まり用の部屋を生成する
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				RoomBase* deadEnd = new RoomBase(this, i, DUNGEONDATA.GetCurrentFloorGenSetting().deadEndRoomType, roomAABBs, isLastRoomGenerated);
				AddRoom(deadEnd);
			}
		}
	}
	// 最後の部屋が生成されていないなら最初のブランチの生成を行う
	else
	{
		// 最大深度が設定値より浅いなら次の部屋を生成する
		if (depth < DUNGEONDATA.GetCurrentFloorGenSetting().maxDepth)
		{
			// 配置時に他の部屋と重ならず、
			// 接続点の先に最後の部屋を生成できるスペースがある部屋を配列に保存する
			std::vector<std::vector<RoomType>> placeableRooms;
			placeableRooms.resize(m_connectPointDatas.size());

			// 接続点の数だけ当たり判定を行い、生成を行う
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				// 最後の部屋生成後に再帰でここに戻ってくることがあるためここで再分岐
				// 最初のブランチの生成
				if (!isLastRoomGenerated)
				{
					for (RoomType type : DUNGEONDATA.GetRoomGenSetting(roomType).placementCandidates)
					{
						// 当たり判定用に子を一次的に生成する
						RoomBase nextRoom = RoomBase(this, i, type);

						// 子のAABBと他のAABB（自分は除く）との衝突判定を行う
						// 子のAABBは少し大きくして部屋が重なって生成されるのを防ぐ
						bool isHit = false;
						for (const AABB& anotherAABB : roomAABBs)
						{
							// AABBの位置が同じなら自分のAABBと判定し処理をスキップする
							if (Mathf::cmpf(anotherAABB.position.x, m_aabb.position.x) &&
								Mathf::cmpf(anotherAABB.position.y, m_aabb.position.y) &&
								Mathf::cmpf(anotherAABB.position.z, m_aabb.position.z)) continue;

							AABB nextRoomAABB = nextRoom.GetAABB();
							IntersectionResult result;

							if (Collision::IntersectAABBVsAABB(
								DirectX::XMLoadFloat3(&nextRoomAABB.position),
								DirectX::XMLoadFloat3(&nextRoomAABB.radii),
								DirectX::XMLoadFloat3(&anotherAABB.position),
								DirectX::XMLoadFloat3(&anotherAABB.radii),
								&result))
							{
								// 他のAABBと衝突したのでこの部屋は配列に保存しない
								isHit = true;
								break;

								// 当たり判定に使用したAABBはデバッグ四角形描画のために保存しておく
								//m_debugAABBs.emplace_back(AABB(nextAABB.position, tmpNextRadii));
							}
						}
						// 他のAABBと衝突しなかった場合はその先に最後の部屋が生成できるかを検証する
						if (!isHit)
						{
							// 子の部屋に接続点が一つ以上ある場合
							if (nextRoom.GetConnectPointDatas().size() > 0)
							{
								// 衝突判定のフラグを接続点の数だけ用意、リサイズする
								std::vector<bool> isHits;
								isHits.resize(nextRoom.GetConnectPointDatas().size());

								for (int j = 0; j < nextRoom.GetConnectPointDatas().size(); j++)
								{
									// 自分のAABBを登録した一次的な配列を用意する
									// 子の部屋（nextRoom）はここでは設定しない
									std::vector<AABB> tmpRoomAABBs;
									tmpRoomAABBs = roomAABBs;
									tmpRoomAABBs.emplace_back(m_aabb);

									// 当たり判定用に最後の部屋を一時的に生成する
									RoomBase endRoom(&nextRoom, j, DUNGEONDATA.GetCurrentFloorGenSetting().endRoomType);

									// 最後の部屋のAABBと他のAABB（子のAABBは除く）との衝突判定を行う
									// 最後の部屋のAABBはちょっと大きくして部屋が干渉するのを防ぐ
									for (const AABB& anotherAABB : tmpRoomAABBs)
									{
										// AABBの位置が同じ==子のAABBと判定して処理をスキップする
										if (Mathf::cmpf(anotherAABB.position.x, nextRoom.GetAABB().position.x) &&
											Mathf::cmpf(anotherAABB.position.y, nextRoom.GetAABB().position.y) &&
											Mathf::cmpf(anotherAABB.position.z, nextRoom.GetAABB().position.z)) continue;

										AABB endRoomAABB = endRoom.GetAABB();
										IntersectionResult result;

										if (Collision::IntersectAABBVsAABB(
											DirectX::XMLoadFloat3(&endRoomAABB.position),
											DirectX::XMLoadFloat3(&endRoomAABB.radii),
											DirectX::XMLoadFloat3(&anotherAABB.position),
											DirectX::XMLoadFloat3(&anotherAABB.radii),
											&result))
										{
											// 他のAABBと衝突したのでこの接続点のフラグを折る
											isHits.at(j) = true;
											break;

											// 当たり判定に使用したAABBはデバッグ四角形描画のために保存しておく
											//m_debugAABBs.emplace_back(AABB(tempEndPos, tempEndAABB));
										}
									}
								}

								for (bool tmpHit : isHits)
								{
									if (tmpHit == true) isHit = true;
								}
								if (!isHit)
								{
									placeableRooms.at(i).emplace_back(type);
								}
							}
						}
					}
				}
				// 通常のブランチの生成
				else
				{
					for (RoomType type : DUNGEONDATA.GetRoomGenSetting(roomType).placementCandidates)
					{
						// 新規生成する子のAABBを算出
						AABB nextAABB = CalcAABB(DUNGEONDATA.GetRoomGenSetting(type).aabb,
							m_connectPointDatas.at(i).position, DirectX::XMConvertToDegrees(m_connectPointDatas.at(i).angle.y));

						// 新規生成する子のAABBと他のAABB（この部屋のAABBは除く）との衝突判定を行う
						// 子のAABBはちょっと大きくして部屋が干渉するのを防ぐ
						bool isHit = false;
						for (const AABB& anotherAABB : roomAABBs)
						{
							// AABBの位置が同じ==自分のAABBと判定して処理をスキップする
							if (Mathf::cmpf(anotherAABB.position.x, m_aabb.position.x) &&
								Mathf::cmpf(anotherAABB.position.y, m_aabb.position.y) &&
								Mathf::cmpf(anotherAABB.position.z, m_aabb.position.z)) continue;

							IntersectionResult result;
							DirectX::XMFLOAT3 tmpNextRadii = {
								nextAABB.radii.x + 4.0f,
								nextAABB.radii.y,
								nextAABB.radii.z + 4.0f
							};
							DirectX::XMFLOAT3 tmpAnotherRadii = anotherAABB.radii;

							if (Collision::IntersectAABBVsAABB(
								DirectX::XMLoadFloat3(&nextAABB.position),
								DirectX::XMLoadFloat3(&tmpNextRadii),
								DirectX::XMLoadFloat3(&anotherAABB.position),
								DirectX::XMLoadFloat3(&tmpAnotherRadii),
								&result))
							{
								// AABBと衝突するなら配列に保存しない
								isHit = true;

								// 当たり判定に使用したAABBはデバッグ四角形描画のために保存しておく
								//m_debugAABBs.emplace_back(AABB(nextAABB.position, tmpNextRadii));

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

				// 当たり判定が終わったところで
				// 自身のAABBを配列に保存
				roomAABBs.emplace_back(m_aabb);

				// 子の部屋を生成する
				// 他の部屋と重ならない部屋があるならば生成を開始する
				if (placeableRooms.at(i).size() > 0)
				{
					// 生成可能な部屋の重みの合計
					int totalWeight = 0;
					for (RoomType type : placeableRooms.at(i))
					{
						totalWeight += DUNGEONDATA.GetRoomGenSetting(type).weight;
					}

					int randomValue = std::rand() % totalWeight;
					for (RoomType type : placeableRooms.at(i))
					{
						randomValue -= DUNGEONDATA.GetRoomGenSetting(type).weight;

						if (randomValue < 0)
						{
							RoomBase* nextRoom = new RoomBase(this, i, type, roomAABBs, isLastRoomGenerated);
							AddRoom(nextRoom);
							break;
						}
					}
				}
				// 何も生成できないならば行き止まり用の部屋を生成する
				else
				{
					RoomBase* deadEnd = new RoomBase(this, i, DUNGEONDATA.GetCurrentFloorGenSetting().deadEndRoomType, roomAABBs, isLastRoomGenerated);
					AddRoom(deadEnd);
				}
			}
		}
		// 一定の深度を超えた場合は最後の部屋を生成する
		else
		{
			// 自身のAABBを配列に保存
			roomAABBs.emplace_back(m_aabb);

			isLastRoomGenerated = true;

			RoomBase* endRoom = new RoomBase(this, 0, DUNGEONDATA.GetCurrentFloorGenSetting().endRoomType, roomAABBs, isLastRoomGenerated);
			AddRoom(endRoom);

			// 他に接続点があれば行き止まり用の部屋を生成する
			if (m_connectPointDatas.size() > 1)
			{
				for (int i = 1; i < m_connectPointDatas.size(); i++)
				{
					RoomBase* deadEnd = new RoomBase(this, i, DUNGEONDATA.GetCurrentFloorGenSetting().deadEndRoomType, roomAABBs, isLastRoomGenerated);
					AddRoom(deadEnd);
				}
			}
		}
	}
}

void RoomBase::GenerateNextRoomFromOrder(
	std::vector<AABB>& roomAABBs,
	std::vector<uint8_t>& roomOrder,
	int& orderIndex)
{
	// 自身のAABBを算出
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// AABB配列に保存
	roomAABBs.emplace_back(m_aabb);

	// 接続点の数だけ子を生成する
	for (int i = 0; i < m_connectPointDatas.size(); i++)
	{
		// もしも配列のサイズを超えてしまうならreturn
		if (orderIndex >= roomOrder.size()) return;

		RoomBase* nextRoom = new RoomBase(this, i, (RoomType)(roomOrder.at(orderIndex - 1)), roomAABBs, roomOrder, ++orderIndex);
		AddRoom(nextRoom);
	}
}

AABB RoomBase::CalcAABB(AABB aabb, DirectX::XMFLOAT3 pos, float degree) const
{
	// 360度以内に丸める
	while (degree >= 360.0f) degree -= 360.0f;
	while (degree < 0.0f) degree += 360.0f;

	AABB buf = aabb;

	// 90度
	if (degree > 89.9f && degree < 90.1f)
	{
		aabb.radii.x = buf.radii.z;
		aabb.radii.z = buf.radii.x;

		aabb.position.x = buf.position.z;
		aabb.position.z = -buf.position.x;
	}

	// 180度
	if (degree > 179.9f && degree < 180.1f)
	{
		aabb.position.x = -buf.position.x;
		aabb.position.z = -buf.position.z;
	}

	// 270度
	if (degree > 269.9f && degree < 270.1f)
	{
		aabb.radii.x = buf.radii.z;
		aabb.radii.z = buf.radii.x;

		aabb.position.x = -buf.position.z;
		aabb.position.z = buf.position.x;
	}

	aabb.position += pos;

	return aabb;
}

void RoomBase::LoadMapData()
{
	// 接続点データのロード
	std::vector<TILE_DATA> connectPointDatas = DUNGEONDATA.GetRoomTileDatas(roomType).at((int)TileType::CONNECTPOINT);
	for (const TILE_DATA& connectPointData : connectPointDatas)
	{
		TILE_DATA resultData;

		// angleは m_angleを足してから
		resultData.angle = connectPointData.angle + m_angle;

		// position はワールド座標に変換してから保存する
		DirectX::XMFLOAT3 pointPos = connectPointData.position;

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR ConnectPointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pointPos), WorldTransform);
		DirectX::XMStoreFloat3(&resultData.position, ConnectPointPos);
		m_connectPointDatas.emplace_back(resultData);
	}
}

void RoomBase::PlaceMapTile(bool isLeader)
{
	std::vector<std::vector<TILE_DATA>> tileDatas = DUNGEONDATA.GetRoomTileDatas(roomType);
	std::vector<SPAWNER_DATA> spawnerDatas = DUNGEONDATA.GetRoomSpawnerDatas(roomType);

	// 通常のタイルの読み込み
	for (int i = (int)TileType::FLOOR_01A; i < (int)TileType::TILETYPE_COUNT; i++)
	{
		// これらはcontinueする　別の場所で使うため
		if ((TileType)i == TileType::PORTAL ||
			(TileType)i == TileType::CONNECTPOINT ||
			(TileType)i == TileType::SPAWNER ||
			(TileType)i == TileType::STAIR_TO_NEXTFLOOR) continue;

		std::vector<FILE_DATA> modelFileDatas;		// 描画用
		std::vector<FILE_DATA> collisionFileDatas;	// 当たり判定用

		// 描画用のデータが登録されているなら取得
		if (DUNGEONDATA.GetModelFileDatas((TileType)i).size() > 0)
		{
			for (FILE_DATA modelFileData : DUNGEONDATA.GetModelFileDatas((TileType)i))
			{
				modelFileDatas.emplace_back(modelFileData);
			}
		}

		// 当たり判定用のデータが登録されているなら取得
		if (DUNGEONDATA.GetCollisionFileDatas((TileType)i).size() > 0)
		{
			for (FILE_DATA collisionFileData : DUNGEONDATA.GetCollisionFileDatas((TileType)i))
			{
				collisionFileDatas.emplace_back(collisionFileData);
			}
		}

		// 描画用タイル生成
		if (modelFileDatas.size() > 0)
		{
			for (const TILE_DATA& tileData : tileDatas.at(i))
			{
				MapTile* modelTile = new MapTile("", 1.0f, this);

				for (const FILE_DATA& data : modelFileDatas)
				{
					if (T_GRAPHICS.isDX11Active)
					{
						modelTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX11, ModelObject::LHS_TOON);
					}
					if (T_GRAPHICS.isDX12Active)
					{
						modelTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX12, ModelObject::LHS_TOON);
					}
				}
				modelTile->SetPosition(tileData.position);
				modelTile->SetAngle(tileData.angle);
				modelTile->SetScale(tileData.scale);
				modelTile->Update(0);

				// ステージの影登録
				if (modelTile->GetModels().size() > 0)
				{
					T_GRAPHICS.GetShadowRenderer()->ModelRegister(modelTile->GetModel(0).get());
				}

				// マネージャーに登録
				GameObjectManager::Instance().Register(modelTile);
			}
		}

		// 当たり判定用タイル生成
		if (collisionFileDatas.size() > 0)
		{
			for (const TILE_DATA& tileData : tileDatas.at(i))
			{
				MapTile* colliderTile = new MapTile("", 1.0f, this);

				for (const FILE_DATA& data : collisionFileDatas)
				{
					if (T_GRAPHICS.isDX11Active)
					{
						colliderTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX11, ModelObject::LHS_TOON);
					}
					if (T_GRAPHICS.isDX12Active)
					{
						colliderTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX12, ModelObject::LHS_PBR);
					}
				}
				if (collisionFileDatas.size() > 0) colliderTile->SetMoveCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);

				// SetCollider後にPos、Angle、Scaleを設定する
				colliderTile->SetPosition(tileData.position);
				colliderTile->SetAngle(tileData.angle);
				colliderTile->SetScale(tileData.scale);
				colliderTile->Update(0);
				colliderTile->Hide();

				MAPTILES.Register(colliderTile);
			}
		}
	}

	// スポナーの読み込み
	for (const SPAWNER_DATA& spawnerData : spawnerDatas)
	{
		// position はワールド座標に変換してから保存する
		DirectX::XMFLOAT3 spawnerPos = spawnerData.tileData.position;

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMVECTOR ResultSpawnerPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&spawnerPos), WorldTransform);
		DirectX::XMFLOAT3 resultSpawnerPos;
		DirectX::XMStoreFloat3(&resultSpawnerPos, ResultSpawnerPos);

		Spawner* spawner = new Spawner(
			spawnerData.enemyType,
			spawnerData.maxExistedEnemiesNum,
			spawnerData.maxSpawnedEnemiesNum);

		spawner->SetPosition(resultSpawnerPos);
		spawner->SetSearchRadius(spawnerData.searchRadius);
		spawner->SetSpawnRadius(spawnerData.spawnRadius);
		spawner->SetSpawnTime(spawnerData.spawnTime);

		SpawnerManager::Instance().Register(spawner);
	}
}

void RoomBase::PlaceTeleporterTile(Stage* stage, Online::OnlineController* onlineController)
{
	std::vector<STAIR_TO_NEXTFLOOR_DATA> stairDatas = DUNGEONDATA.GetStairToNextFloorDatas(roomType);

	for (const STAIR_TO_NEXTFLOOR_DATA& stairData : stairDatas)
	{
		// ボスフロアでないなら次の階への階段
		if (!(DUNGEONDATA.GetCurrentFloor() >= DUNGEONDATA.GetDungeonGenSetting().maxFloor))
		{
			// position はワールド座標に変換してから保存する
			DirectX::XMFLOAT3 stairPos = stairData.tileData.position;

			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
			DirectX::XMVECTOR ResultStairPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&stairPos), WorldTransform);
			DirectX::XMFLOAT3 resultStairPos;
			DirectX::XMStoreFloat3(&resultStairPos, ResultStairPos);

			StairToNextFloor* stair = new StairToNextFloor(stage, onlineController);
			stair->SetPosition(resultStairPos);
			stair->SetInteractionDistance(stairData.interactionDistance);

			GameObjectManager::Instance().Register(stair);
		}
		// ボスフロアならOpenWorldへのテレポーターと階段を配置
		else
		{
			// テレポーター
			DirectX::XMFLOAT3 stairPos = stairData.tileData.position;

			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
			DirectX::XMVECTOR ResultStairPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&stairPos), WorldTransform);
			DirectX::XMFLOAT3 resultStairPos;
			DirectX::XMStoreFloat3(&resultStairPos, ResultStairPos);

			TeleportToOpenworld* teleporter = new TeleportToOpenworld();
			teleporter->SetPosition(resultStairPos);
			teleporter->SetInteractionDistance(stairData.interactionDistance);
			GameObjectManager::Instance().Register(teleporter);

			// 階段モデル（当たり判定なし）
			MapTile* modelTile = new MapTile("", 1.0f, this);

			std::vector<FILE_DATA> modelFileDatas = DUNGEONDATA.GetModelFileDatas(TileType::STAIR_STEP_01A);
			for (const FILE_DATA& data : modelFileDatas)
			{
				if (T_GRAPHICS.isDX11Active)
				{
					modelTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX11, ModelObject::LHS_TOON);
				}
				if (T_GRAPHICS.isDX12Active)
				{
					modelTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX12, ModelObject::LHS_TOON);
				}
			}
			modelTile->SetPosition({ -2.0f, 0.0f, 122.0f });
			modelTile->SetAngle({ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f });
			modelTile->SetScale({ 1.0f, 1.0f, 1.0f });
			modelTile->Update(0);
			// ステージの影登録
			if (modelTile->GetModels().size() > 0)
			{
				T_GRAPHICS.GetShadowRenderer()->ModelRegister(modelTile->GetModel(0).get());
			}
			GameObjectManager::Instance().Register(modelTile);
		}
	}
}

int RoomBase::DrawDebugGUI(int i)
{
	std::string nameStr = "";

	if (ImGui::TreeNode((nameStr + "(" + std::to_string(i) + ")").c_str()))
	{
		// プロパティ
		if (ImGui::TreeNode("Properties"))
		{
			ImGui::DragFloat3("Position", &m_position.x);
			DirectX::XMFLOAT3 debugAngle;
			debugAngle.x = DirectX::XMConvertToDegrees(m_angle.x);
			debugAngle.y = DirectX::XMConvertToDegrees(m_angle.y);
			debugAngle.z = DirectX::XMConvertToDegrees(m_angle.z);
			ImGui::DragFloat3("Angle", &debugAngle.x);
			m_angle.x = DirectX::XMConvertToRadians(debugAngle.x);
			m_angle.y = DirectX::XMConvertToRadians(debugAngle.y);
			m_angle.z = DirectX::XMConvertToRadians(debugAngle.z);
			ImGui::DragFloat3("Scale", &m_scale.x);

			float newWindowWidth = (ImGui::GetWindowWidth() * 0.15f);
			ImGui::Text("Transform"); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_11", &m_transform._11); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_12", &m_transform._12); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_13", &m_transform._13); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_14", &m_transform._14); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_21", &m_transform._21); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_22", &m_transform._22); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_23", &m_transform._23); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_24", &m_transform._24); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_31", &m_transform._31); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_32", &m_transform._32); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_33", &m_transform._33); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_34", &m_transform._34); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_41", &m_transform._41); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_42", &m_transform._42); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_43", &m_transform._43); ImGui::SameLine(); ImGui::SetNextItemWidth(newWindowWidth);
			ImGui::DragFloat("##_44", &m_transform._44); ImGui::SetNextItemWidth(newWindowWidth);

			ImGui::Text(("TileDataCount: " + std::to_string(m_tileDatas.size())).c_str());
			ImGui::Text(("ConnectPointCount: " + std::to_string(m_connectPointDatas.size())).c_str());
			ImGui::Text(("ParentConnectPointIndex: " + std::to_string(parentConnectPointIndex)).c_str());
			ImGui::Text(("Depth: " + std::to_string(depth)).c_str());
			ImGui::DragFloat3("AABB.pos", &m_aabb.position.x);
			ImGui::DragFloat3("AABB.radii", &m_aabb.radii.x);

			ImGui::TreePop();
		}

		// 接続点
		if (m_connectPointDatas.size() > 0)
		{
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
		}

		// 子
		if (childs.size() > 0)
		{
			if (ImGui::TreeNode("Childs"))
			{
				for (RoomBase* child : childs)
				{
					i = child->DrawDebugGUI(++i);
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	return i;
}