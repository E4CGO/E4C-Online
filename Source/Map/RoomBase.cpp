#include "Source/Map/RoomBase.h"

#include <TAKOEngine\Tool\XMFLOAT.h>
#include <imgui.h>

#include "Map/SimpleRoom1.h"
#include "Map/EndRoom1.h"
#include "Map/CrossRoom1.h"
#include "Map/CrossRoom2.h"
#include "Map/Passage1.h"
#include "Map/DeadEndRoom.h"

#include "MapTile.h"
#include "MapTileManager.h"

RoomBase::RoomBase(
	RoomBase* parent, int pointIndex,
	std::vector<AABB>& roomAABBs,
	bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
{
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

	// 深度を取得
	depth = GetDepth();

	// 後の接続点データ設定、部屋モデル配置のために行列更新処理を行う
	UpdateTransform();
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

void RoomBase::GenerateNextRoom(
	std::vector<AABB>& roomAABBs,
	bool isAutoGeneration,
	std::vector<uint8_t>& roomOrder, int& orderIndex)
{
	DungeonData& dungeonData = DungeonData::Instance();

	// 部屋データのロード（接続点データ設定もここで行う）
	LoadMapData();

	// 親がない＝最初の部屋ならば入口を塞いでおく
	if (parent == nullptr)
	{
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ -2.0f, 0.0f, -2.0f },
			{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::WALL).emplace_back(TILE_DATA(
			{ -2.0f, 3.0f, -2.0f },
			{ 0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f },
			{ 1.0f, 1.0f, 1.0f }));

		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ -2.0f, 0.0f, -2.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ -2.0f, 3.0f, -2.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ 2.0f, 0.0f, -2.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
		m_tileDatas.at(TileType::PILLAR).emplace_back(TILE_DATA(
			{ 2.0f, 3.0f, -2.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }));
	}

	// 自身のAABBを算出
	m_aabb = CalcAABB(dungeonData.GetRoomGenerateSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// AABB配列に保存
	roomAABBs.emplace_back(m_aabb);



	// 自動生成を行う
	if (isAutoGeneration)
	{
		// 最大深度が設定値より浅いなら次の部屋を生成する
		if (depth < dungeonData.GetDungeonGenerateSetting().maxDepth)
		{
			// 配置時に他の部屋と重ならない部屋のみを配列に保存する
			std::vector<std::vector<RoomType>> placeableRooms;
			placeableRooms.resize(m_connectPointDatas.size());

			// 接続点の数だけ当たり判定を行い、生成を行う
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				for (RoomType type : dungeonData.GetRoomGenerateSetting(roomType).placementCandidates)
				{
					AABB nextRoomAABB = CalcAABB(dungeonData.GetRoomGenerateSetting(type).aabb,
						m_connectPointDatas.at(i).position, DirectX::XMConvertToDegrees(m_connectPointDatas.at(i).angle.y));

					bool isHit = false;

					// 自分以外のAABBとの当たり判定を行う
					for (const AABB& anotherRoomAABB : roomAABBs)
					{
						IntersectionResult result;

						// 部屋同士の当たり判定時のみ半径を少しだけ伸ばすことで、複数の部屋が隣接し、一つの部屋のように生成されてしまうのを防ぐ
						DirectX::XMFLOAT3 temp_NextRadii =
						{
							nextRoomAABB.radii.x + 0.0f,
							nextRoomAABB.radii.y + 0.0f,
							nextRoomAABB.radii.z + 0.0f,
						};

						DirectX::XMFLOAT3 temp_AnotherRadii =
						{
							anotherRoomAABB.radii.x + 0.0f,
							anotherRoomAABB.radii.y + 0.0f,
							anotherRoomAABB.radii.z + 0.0f,
						};

						if (Collision::IntersectAABBVsAABB(
							DirectX::XMLoadFloat3(&nextRoomAABB.position),
							DirectX::XMLoadFloat3(&temp_NextRadii),
							DirectX::XMLoadFloat3(&anotherRoomAABB.position),
							DirectX::XMLoadFloat3(&temp_AnotherRadii),
							&result))
						{
							// AABBと衝突するなら配列に保存しない
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

				// 子の部屋を生成する
				// 他の部屋と重ならない部屋があるならば生成を開始する
				if (placeableRooms.at(i).size() > 0)
				{
					// 生成可能な部屋の重みの合計
					int totalWeight = 0;
					for (RoomType type : placeableRooms.at(i))
					{
						totalWeight += dungeonData.GetRoomGenerateSetting(type).weight;
					}

					int randomValue = std::rand() % totalWeight;
					for (RoomType type : placeableRooms.at(i))
					{
						randomValue -= dungeonData.GetRoomGenerateSetting(type).weight;

						if (randomValue < 0)
						{
							RoomBase* nextRoom = nullptr;

							switch (type)
							{
							case RoomType::SIMPLE_ROOM_1:
								nextRoom = new SimpleRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
								break;

							case RoomType::END_ROOM:
								nextRoom = new EndRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
								break;

							case RoomType::CROSS_ROOM_1:
								nextRoom = new CrossRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
								break;

							case RoomType::CROSS_ROOM_2:
								nextRoom = new CrossRoom2(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
								break;

							case RoomType::PASSAGE_1:
								nextRoom = new Passage1(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
								break;

							default:
								break;
							}
							AddRoom(nextRoom);
							break;
						}
					}
				}
				// 何も生成できないならば行き止まり用の部屋を生成する
				else
				{
					//EndRoom1* end = new EndRoom1(this, i);
					//AddRoom(end);
					DeadEndRoom* deadEnd = new DeadEndRoom(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
					AddRoom(deadEnd);
				}
			}
		}
		// 一定の深度を超えた場合は行き止まり用の部屋を生成する
		else
		{
			// 接続点の数だけ行き止まり用の部屋を生成する
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				DeadEndRoom* deadEnd = new DeadEndRoom(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
				AddRoom(deadEnd);
			}
		}
	}
	// 自動生成は行わず、生成順（roomOrder）で生成を行う
	else
	{
		// 接続点の数だけ子を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			// もしも配列のサイズを超えてしまうならreturn
			if (orderIndex >= roomOrder.size()) return;

			uint8_t nextRoomType = roomOrder.at(orderIndex);

			RoomBase* nextRoom = nullptr;

			switch (nextRoomType)
			{
			case RoomType::SIMPLE_ROOM_1:
				nextRoom = new SimpleRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, ++orderIndex);
				break;

			case RoomType::END_ROOM:
				nextRoom = new EndRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, ++orderIndex);
				break;

			case RoomType::CROSS_ROOM_1:
				nextRoom = new CrossRoom1(this, i, roomAABBs, isAutoGeneration, roomOrder, ++orderIndex);
				break;

			case RoomType::CROSS_ROOM_2:
				nextRoom = new CrossRoom2(this, i, roomAABBs, isAutoGeneration, roomOrder, ++orderIndex);
				break;

			case RoomType::PASSAGE_1:
				nextRoom = new Passage1(this, i, roomAABBs, isAutoGeneration, roomOrder, ++orderIndex);
				break;

			case RoomType::DEAD_END:
				nextRoom = new DeadEndRoom(this, i, roomAABBs, isAutoGeneration, roomOrder, orderIndex);
				break;
			}
			AddRoom(nextRoom);
		}
	}
}


AABB RoomBase::CalcAABB(AABB aabb, DirectX::XMFLOAT3 pos, float degree) const
{
	// 360度以内に丸める
	while (degree >= 360.0f) degree -= 360.0f;
	while (degree < 0.0f) degree += 360.0f;

	// 角度によって補正を行う
	// 90度か270度ならxとzを逆転させる
	if ((degree > 89.9f && degree < 90.1f) || (degree > 269.9f && degree < 270.1f))
	{
		AABB buf = aabb;

		aabb.position.x = aabb.position.z;
		aabb.position.z = buf.position.x;
		aabb.radii.x = aabb.radii.z;
		aabb.radii.z = buf.radii.x;
	}

	// 90度
	if (degree > 89.9f && degree < 90.1f)
	{

	}

	// 180度
	if (degree > 179.9f && degree < 180.1f)
	{
		// 位置補正
		//aabb.position.z += tileScale;

		// zを反転
		aabb.position.z = -aabb.position.z;
	}

	// 270度
	if (degree > 269.9f && degree < 270.1f)
	{
		// xを反転
		aabb.position.x = -aabb.position.x;

		// 位置補正
		//aabb.position.x += tileScale;
	}

	aabb.position += pos;

	return aabb;
}

void RoomBase::PlaceMapTile(bool isLeader)
{
	struct ModelFileData
	{
		std::string fileName;
		float scale = 1.0;
	};

	for (int tileType = TileType::FLOOR; tileType < TileType::TILETYPE_COUNT; tileType++)
	{
		std::vector<ModelFileData> colliderFileNames;	// 当たり判定用モデル
		std::vector<ModelFileData> modelFileNames;		// 描画用モデル

		switch (tileType)
		{
		case TileType::FLOOR:
			colliderFileNames.emplace_back("Data/Model/DungeonAssets/NewAssets/FloorCollision_01a.glb", 4.0f);
			modelFileNames.emplace_back("Data/Model/DungeonAssets/NewAssets/SM_Floor_01a.glb", 4.0f);
			break;
		case TileType::WALL:
			colliderFileNames.emplace_back("Data/Model/DungeonAssets/NewAssets/WallCollision_01a.glb", 4.0f);
			modelFileNames.emplace_back("Data/Model/DungeonAssets/NewAssets/SM_Wall_Pattern_01a.glb", 4.0f);
			break;
		case TileType::PILLAR:
			continue;
		case TileType::STAIR:
			colliderFileNames.emplace_back("Data/Model/DungeonAssets/SLOPE.glb");
			break;
		case TileType::SPAWNER:
			continue;
		}

		for (const TILE_DATA& tileData : m_tileDatas.at(tileType))
		{
			MapTile* newTile = new MapTile("", 1.0f, this);

			// 当たり判定モデルだけ先に読み込み、当たり判定を設定する
			for (const ModelFileData& data : colliderFileNames)
			{
				if (T_GRAPHICS.isDX11Active)
				{
					newTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX11, ModelObject::LHS_TOON);
				}
				if (T_GRAPHICS.isDX12Active)
				{
					newTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX12, ModelObject::LHS_PBR);
				}
			}

			if (colliderFileNames.size() != 0) newTile->SetCollider(Collider::COLLIDER_TYPE::MAP);

			// 表示用モデルは後に読み込む
			for (const ModelFileData& data : modelFileNames)
			{
				if (T_GRAPHICS.isDX11Active)
				{
					newTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX11, ModelObject::LHS_TOON);
				}
				if (T_GRAPHICS.isDX12Active)
				{
					newTile->LoadModel(data.fileName.c_str(), data.scale, ModelObject::RENDER_MODE::DX12, ModelObject::LHS_PBR);
				}
			}
			newTile->SetPosition(tileData.position);
			newTile->SetAngle(tileData.angle);
			newTile->SetScale(tileData.scale);
			newTile->SetColor(tileData.color);
			newTile->Update(0);
			MAPTILES.Register(newTile);
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