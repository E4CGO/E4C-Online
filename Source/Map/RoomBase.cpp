#include "Source/Map/RoomBase.h"

#include <TAKOEngine\Tool\XMFLOAT.h>
#include <imgui.h>

#include "Map/SimpleRoom1.h"
#include "Map/EndRoom1.h"
#include "Map/CrossRoom1.h"
#include "Map/Passage1.h"

#include "MapTile.h"
#include "MapTileManager.h"

RoomBase::RoomBase(RoomBase* parent, int pointIndex)
{
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
		// スケール行列生成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		// 回転行列生成
		DirectX::XMMATRIX R = AnglesToMatrix(m_angle);
		// 位置行列生成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		DirectX::XMMATRIX LocalTransform = S * R * T;

		DirectX::XMMATRIX ParentTransform;
		//if (parentConnectPointIndex > -1)
		//{
		//	DirectX::XMFLOAT4X4 parentTransform = parent->GetConnectPointData(parentConnectPointIndex).transform;
		//	ParentTransform = DirectX::XMLoadFloat4x4(&parentTransform);
		//}
		//else
		{
			ParentTransform = DirectX::XMMatrixIdentity();
		}
		DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;

		DirectX::XMStoreFloat4x4(&m_transform, GlobalTransform);
	}

	// 接続点データも更新
	for (CONNECTPOINT_DATA& data : m_connectPointDatas)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		DirectX::XMMATRIX R = AnglesToMatrix(data.angle);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(data.position.x, data.position.y, data.position.z);

		DirectX::XMMATRIX LocalTransform = S * R * T;
		DirectX::XMMATRIX ParentTransform = DirectX::XMLoadFloat4x4(&m_transform);
		DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;
		DirectX::XMStoreFloat4x4(&data.transform, GlobalTransform);
	}
}

void RoomBase::GenerateNextRoom()
{
	DungeonData& dungeonData = DungeonData::Instance();

	// 部屋データのロード（接続点データ設定もここで行う）
	LoadMapData();

	if (depth == 1 && this->roomType == DungeonData::CROSS_ROOM_1)
	{
		int a = 0;
	}

	// 自身のAABBを算出
	m_aabb = CalcAABB(dungeonData.GetRoomGenerateSetting(roomType).aabb,
		m_position, DirectX::XMConvertToDegrees(m_angle.y));

	// 配列に登録
	dungeonData.AddRoomAABBs(m_aabb);

	// 部屋の生成配列が存在しないなら新しくダンジョンの自動生成を行う
	if (dungeonData.GetRoomTree().size() < 1)
	{
		// 最大深度が設定値より浅いなら次の部屋を生成する
		if (depth < dungeonData.GetDungeonGenerateSetting().maxDepth)
		{
			// 配置時に他の部屋と重ならない部屋のみを配列に保存する
			std::vector<std::vector<DungeonData::RoomType>> placeableRooms;
			placeableRooms.resize(m_connectPointDatas.size());

			// 接続点の数だけ当たり判定を行い、生成を行う
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				std::vector<AABB> aAABBs = dungeonData.GetRoomAABBs();

				for (DungeonData::RoomType type : dungeonData.GetRoomGenerateSetting(roomType).placementCandidates)
				{
					AABB nextRoomAABB = CalcAABB(dungeonData.GetRoomGenerateSetting(type).aabb,
						m_connectPointDatas.at(i).position, DirectX::XMConvertToDegrees(m_connectPointDatas.at(i).angle.y));

					bool isHit = false;

					// 自分以外のAABBとの当たり判定を行う
					for (const AABB& anotherRoomAABB : dungeonData.GetRoomAABBs())
					{
						IntersectionResult result;

						// 部屋同士の当たり判定時のみ半径を少しだけ伸ばすことで、複数の部屋が隣接し、一つの部屋のように生成されてしまうのを防ぐ
						DirectX::XMFLOAT3 temp_NextRadii =
						{
							nextRoomAABB.radii.x + 1.0f,
							nextRoomAABB.radii.y + 1.0f,
							nextRoomAABB.radii.z + 1.0f,
						};

						DirectX::XMFLOAT3 temp_AnotherRadii =
						{
							anotherRoomAABB.radii.x + 1.0f,
							anotherRoomAABB.radii.y + 1.0f,
							anotherRoomAABB.radii.z + 1.0f,
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
					for (DungeonData::RoomType type : placeableRooms.at(i))
					{
						totalWeight += dungeonData.GetRoomGenerateSetting(type).weight;
					}

					int randomValue = std::rand() % totalWeight;
					for (DungeonData::RoomType type : placeableRooms.at(i))
					{
						randomValue -= dungeonData.GetRoomGenerateSetting(type).weight;

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

			//// 接続点の数だけ子を生成する
			//for (int i = 0; i < m_connectPointDatas.size(); i++)
			//{
			//	// 他の部屋と重ならない部屋が存在しない場合は処理を行わない
			//	if (placeableRooms.at(i).size() < 1) continue;

			//	// 接続可能な部屋の重みの合計
			//	int totalWeight = 0;
			//	for (DungeonData::RoomType type : placeableRooms.at(i))
			//	{
			//		totalWeight += dungeonData.GetRoomGenerateSetting(type).weight;
			//	}

			//	int randomValue = std::rand() % totalWeight;
			//	for (DungeonData::RoomType type : placeableRooms.at(i))
			//	{
			//		randomValue -= dungeonData.GetRoomGenerateSetting(type).weight;

			//		if (randomValue < 0)
			//		{
			//			RoomBase* nextRoom = nullptr;

			//			switch (type)
			//			{
			//			case DungeonData::SIMPLE_ROOM_1:
			//				nextRoom = new SimpleRoom1(this, i);
			//				break;

			//			case DungeonData::END_ROOM:
			//				nextRoom = new EndRoom1(this, i);
			//				break;

			//			case DungeonData::CROSS_ROOM_1:
			//				nextRoom = new CrossRoom1(this, i);
			//				break;

			//			case DungeonData::PASSAGE_1:
			//				//nextRoom = new Passage1(this, i, roomAABBs);
			//				break;

			//			default:
			//				break;
			//			}
			//			AddRoom(nextRoom);
			//			break;
			//		}
			//	}
			//}
		}
		// 一定の深度を超えた場合は終端の部屋を生成する
		else
		{
			// 接続点の数だけ終端の部屋を生成する
			for (int i = 0; i < m_connectPointDatas.size(); i++)
			{
				//RoomBase* nextEndRoom = new EndRoom1(this, i, roomAABBs);
				//AddRoom(nextEndRoom);
			}
		}

	}
	// 存在するなら生成配列に従った生成を行う
	else
	{
		// 接続点の数だけ子を生成する
		for (int i = 0; i < m_connectPointDatas.size(); i++)
		{
			/*
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
			*/
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
		aabb.position.z += tileScale;

		// zを反転
		aabb.position.z = -aabb.position.z;
	}

	// 270度
	if (degree > 269.9f && degree < 270.1f)
	{
		// xを反転
		aabb.position.x = -aabb.position.x;

		// 位置補正
		aabb.position.x += tileScale;
	}

	aabb.position += pos;

	return aabb;
}

void RoomBase::PlaceMapTile()
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
			//fileName = "Data/Model/Dungeon/DoorWay Parent 006.glb";
			//continue;
			break;
		case TileType::STAIR:
			fileName = "Data/Model/Dungeon/Stair Parent 001.glb";
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
	}
}

int RoomBase::DrawDebugGUI(int i)
{
	std::string nameStr = "";

	switch (roomType)
	{
	case DungeonData::SIMPLE_ROOM_1: nameStr = "SimpleRoom1";	break;
	case DungeonData::END_ROOM:		 nameStr = "EndRoom";		break;
	case DungeonData::CROSS_ROOM_1:	 nameStr = "CrossRoom1";	break;
	case DungeonData::PASSAGE_1:	 nameStr = "Passage1";		break;
	default: break;
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

			ImGui::Text(("TileDataCount: " + std::to_string(m_tileDatas.size())).c_str());
			ImGui::Text(("ConnectPointCount: " + std::to_string(m_connectPointDatas.size())).c_str());
			//ImGui::Text(("MapTileSize: " + std::to_string(mapTiles.size())).c_str());
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