#include "Source/Map/RoomBase.h"

#include <TAKOEngine\Tool\XMFLOAT.h>
#include <filesystem>
#include <imgui.h>

#include "GameObject/GameObjectManager.h"
#include "GameObject/Props/Spawner.h"
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

	// 次の部屋の生成を行う
	GenerateNextRoomAutomatically(roomAABBs, isLastRoomGenerated);
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

	// 次の部屋の生成を行う
	GenerateNextRoomFromOrder(roomAABBs, roomOrder, orderIndex);
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

	//// 接続点データも更新
	//for (CONNECTPOINT_DATA& data : m_connectPointDatas)
	//{
	//	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	//	DirectX::XMMATRIX R = AnglesToMatrix(data.angle);
	//	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(data.position.x, data.position.y, data.position.z);

	//	DirectX::XMMATRIX LocalTransform = S * R * T;
	//	DirectX::XMMATRIX ParentTransform = DirectX::XMLoadFloat4x4(&m_transform);
	//	DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;
	//	DirectX::XMStoreFloat4x4(&data.transform, GlobalTransform);
	//}
}

DirectX::XMFLOAT3 RoomBase::GetCenterPos()
{
	DirectX::XMFLOAT3 placePos = DungeonData::Instance().GetRoomGenerateSetting(roomType).portalPosition;

	float degree = DirectX::XMConvertToDegrees(m_angle.y);

	// 360度以内に丸める
	while (degree >= 360.0f) degree -= 360.0f;
	while (degree < 0.0f) degree += 360.0f;

	// 角度によって補正を行う
	// 90度か270度ならxとzを逆転させる
	if ((degree > 89.9f && degree < 90.1f) || (degree > 269.9f && degree < 270.1f))
	{
		DirectX::XMFLOAT3 buf = placePos;
		placePos.x = placePos.z;
		placePos.z = buf.x;
	}

	// 90度
	if (degree > 89.9f && degree < 90.1f)
	{

	}

	// 180度
	if (degree > 179.9f && degree < 180.1f)
	{
		// 位置補正
		//placePos += tileScale;

		// zを反転
		placePos.z = -placePos.z;
	}

	// 270度
	if (degree > 269.9f && degree < 270.1f)
	{
		// xを反転
		placePos.x = -placePos.x;

		// 位置補正
		//placePos.x += tileScale;
	}
	return m_position + placePos;
}

void RoomBase::GenerateNextRoomAutomatically(
	std::vector<AABB>& roomAABBs,
	bool& isLastRoomGenerated)
{
	// 自身のAABBを算出
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenerateSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// 最大深度が設定値より浅いなら次の部屋を生成する
	if (depth < DUNGEONDATA.GetDungeonGenerateSetting().maxDepth)
	{
		// 配置時に他の部屋と重ならない部屋のみを配列に保存する
		std::vector<std::vector<RoomType>> placeableRooms;
		placeableRooms.resize(m_connectPointDatas.size());

		// 接続点の数だけ当たり判定を行い、生成を行う
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			for (RoomType type : DUNGEONDATA.GetRoomGenerateSetting(roomType).placementCandidates)
			{
				// 新規生成する子のAABBを算出
				AABB nextAABB = CalcAABB(DUNGEONDATA.GetRoomGenerateSetting(type).aabb,
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
						m_debugAABBs.emplace_back(AABB(nextAABB.position, tmpNextRadii));

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
					totalWeight += DUNGEONDATA.GetRoomGenerateSetting(type).weight;
				}

				int randomValue = std::rand() % totalWeight;
				for (RoomType type : placeableRooms.at(i))
				{
					randomValue -= DUNGEONDATA.GetRoomGenerateSetting(type).weight;

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
				RoomBase* deadEnd = new RoomBase(this, i, RoomType::DEAD_END, roomAABBs, isLastRoomGenerated);
				AddRoom(deadEnd);
			}
		}
	}
	// 一定の深度を超えた場合は行き止まり用の部屋を生成する
	else
	{
		// 自身のAABBを配列に保存
		roomAABBs.emplace_back(m_aabb);

		//if (!isLastRoomGenerated)
		//{
		//	if (m_connectPointDatas.size() == 0) return;

		//	isLastRoomGenerated = true;

		//	RoomBase* endRoom = new RoomBase(this, 0, RoomType::FIRST_END, roomAABBs, isLastRoomGenerated);

		//	if (m_connectPointDatas.size() > 1)
		//	{
		//		for (int i = 1; i < m_connectPointDatas.size(); i++)
		//		{
		//			RoomBase* deadEnd = new RoomBase(this, i, RoomType::DEAD_END, roomAABBs, isLastRoomGenerated);
		//			AddRoom(deadEnd);
		//		}
		//	}
		//}
		//else
		{
			// 接続点の数だけ行き止まり用の部屋を生成する
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				RoomBase* deadEnd = new RoomBase(this, i, RoomType::DEAD_END, roomAABBs, isLastRoomGenerated);
				AddRoom(deadEnd);
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
	m_aabb = CalcAABB(DUNGEONDATA.GetRoomGenerateSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// AABB配列に保存
	roomAABBs.emplace_back(m_aabb);

	// 接続点の数だけ子を生成する
	for (int i = 0; i < m_connectPointDatas.size(); i++)
	{
		// もしも配列のサイズを超えてしまうならreturn
		if (orderIndex >= roomOrder.size()) return;

		RoomBase* nextRoom = new RoomBase(this, i, (RoomType)(roomOrder.at(orderIndex)), roomAABBs, roomOrder, ++orderIndex);
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
		aabb.position.z = buf.position.x;
	}

	// 180度
	if (degree > 179.9f && degree < 180.1f)
	{
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
	nlohmann::json loadFile;
	std::ifstream ifs(DUNGEONDATA.GetFileName(roomType));

	if (ifs.is_open())
	{
		ifs >> loadFile;

		// ノードデータロード
		for (const auto& nodeData : loadFile["NodeDatas"])
		{
			TileType tileType = nodeData["Type"];

			switch (tileType)
			{
			case ns_RoomData::PORTAL: continue;

			case ns_RoomData::SPAWNER:
			{
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

				TILE_DATA newData;
				newData.position = DirectX::XMFLOAT3(position);
				newData.angle = m_angle + angle;

				// ワールド座標に変換し保存
				DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
				DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newData.position), WorldTransform);
				DirectX::XMStoreFloat3(&newData.position, PointPos);
				m_tileDatas.at(tileType).emplace_back(TILE_DATA(
					position,
					angle,
					{ 1.0f, 1.0f, 1.0f })
				);
			}
			continue;

			case ns_RoomData::CONNECTPOINT:
			{
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

				TILE_DATA newPoint;
				newPoint.position = DirectX::XMFLOAT3(position);
				newPoint.angle = m_angle + angle;

				// ワールド座標に変換し保存
				DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&m_transform);
				DirectX::XMVECTOR PointPos = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&newPoint.position), WorldTransform);
				DirectX::XMStoreFloat3(&newPoint.position, PointPos);
				m_connectPointDatas.emplace_back(newPoint);
			}
			continue;

			default:
				break;
			}

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
}

void RoomBase::PlaceMapTile(bool isLeader)
{
	for (uint8_t tileType = TileType::FLOOR_01A; tileType < TileType::TILETYPE_COUNT; tileType++)
	{
		if (m_tileDatas.at(tileType).size() < 1) continue;

		// Spawnerは特殊
		if (tileType == TileType::SPAWNER)
		{
			for (const TILE_DATA& data : m_tileDatas.at(tileType))
			{
				Spawner* spawner = new Spawner(2, 2, -1);
				spawner->SetPosition(m_position + data.position);

				GameObjectManager::Instance().Register(spawner);
			}
			continue;
		}

		// ConnectPointはコンストラクタで既に読込み済だからcontinue
		if (tileType == TileType::CONNECTPOINT) continue;

		std::vector<FILE_DATA> collisionFileDatas;	// 当たり判定用
		std::vector<FILE_DATA> modelFileDatas;		// 描画用

		// 当たり判定用のデータが登録されているなら取得
		if (DUNGEONDATA.GetCollisionFileDatas((TileType)tileType).size() > 0)
		{
			collisionFileDatas.emplace_back(DUNGEONDATA.GetCollisionFileDatas((TileType)tileType).at(0));
		}

		// 描画用のデータが登録されているなら取得
		if (DUNGEONDATA.GetModelFileDatas((TileType)tileType).size() > 0)
		{
			modelFileDatas.emplace_back(DUNGEONDATA.GetModelFileDatas((TileType)tileType).at(0));
		}

		// 描画用データがあるなら
		if (modelFileDatas.size() > 0)
		{
			// ここのコメントアウトを解除するとインスタンシング

			// インスタンシング
			//if (T_GRAPHICS.isDX12Active &&
			//	tileType == TileType::FLOOR_01A &&
			//	tileType == TileType::FLOOR_01B &&
			//	tileType == TileType::FLOOR_02A &&
			//	tileType == TileType::FLOOR_03A &&
			//	tileType == TileType::ARCH_FLOOR_01A &&
			//	tileType == TileType::FLOOR_CLOUD_01A)
			//{
			//	FILE_DATA fileData = { modelFileDatas.at(0).fileName, modelFileDatas.at(0).scale };

			//	std::filesystem::path filePath = fileData.fileName;
			//	std::string fileNameStr = filePath.stem().string();
			//	const char* fileName = fileNameStr.c_str();

			//	ModelObject* instancingModel = new ModelObject(
			//		fileData.fileName.c_str(),
			//		fileData.scale,
			//		ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_TOON);
			//	instancingModel->SetShader(fileName, ModelShaderDX12Id::ToonInstancing);

			//	DirectX::XMMATRIX LeftHandScaling = DirectX::XMMatrixScaling(-1, 1, 1);

			//	for (int i = 0; i < m_tileDatas.at(tileType).size(); ++i)
			//	{
			//		int id = instancingModel->GetModel()->AllocateInstancingIndex();
			//		if (id < 0) continue;

			//		DirectX::XMFLOAT3 position = m_tileDatas.at(tileType).at(i).position;
			//		DirectX::XMFLOAT3 angle = m_tileDatas.at(tileType).at(i).angle;
			//		DirectX::XMFLOAT3 scale = m_tileDatas.at(tileType).at(i).scale;

			//		// 床ならangleX = 0.0f
			//		// 壁ならangleX = 90.0f
			//		// angleYをマイナスに
			//		// positionXをマイナスに

			//		// スケール行列生成
			//		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x * fileData.scale, scale.y * fileData.scale, scale.z * fileData.scale);
			//		// 回転行列生成
			//		float angleX = 0.0f;
			//		DirectX::XMMATRIX R = DirectX::XMMatrixIdentity();
			//		if (tileType == TileType::WALL_01A)
			//		{
			//			angleX = 0.0f;
			//			DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angleX));
			//			DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
			//			DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);

			//			R = X * Y * Z;
			//		}
			//		// 位置行列生成
			//		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(-position.x, position.y, position.z);

			//		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&m_transform);

			//		DirectX::XMMATRIX W = (S * R * T) * P * LeftHandScaling;

			//		DirectX::XMFLOAT4X4 tm;
			//		DirectX::XMStoreFloat4x4(&tm, W);
			//		instancingModel->GetModel()->UpdateTransform(id, tm);
			//	}
			//	MAPTILES.Register(instancingModel);
			//}
			//// 通常の読み込み
			//else
			{
				for (const TILE_DATA& tileData : m_tileDatas.at(tileType))
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

					// 当たり判定データが無い場合は当たり判定としても使う
					if (collisionFileDatas.size() <= 0)
					{
						MAPTILES.Register(modelTile);
					}
					else
					{
						GameObjectManager::Instance().Register(modelTile);
					}
				}
			}
		}

		// 当たり判定用データがあるなら
		if (collisionFileDatas.size() > 0)
		{
			for (const TILE_DATA& tileData : m_tileDatas.at(tileType))
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
				if (collisionFileDatas.size() != 0) colliderTile->SetCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);

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
}

int RoomBase::DrawDebugGUI(int i)
{
	std::string nameStr = "";

	switch (roomType)
	{
	case RoomType::SIMPLE_ROOM_1:	nameStr = "SimpleRoom1";	break;
	case RoomType::END_ROOM:		nameStr = "EndRoom";		break;
	case RoomType::CROSS_ROOM_1:	nameStr = "CrossRoom1";		break;
	case RoomType::PASSAGE_1:		nameStr = "Passage1";		break;
	case RoomType::DEAD_END:		nameStr = "DeadEnd";		break;
	}

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