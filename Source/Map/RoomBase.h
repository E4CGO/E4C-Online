#pragma once

#include "Map/DungeonData.h"
#include <vector>

// 部屋ベース
class RoomBase
{
public:
	// コンストラクタ
	RoomBase() = default;
	virtual ~RoomBase()
	{
		for (RoomBase* room : childs)
		{
			if (room != nullptr) delete room;
		}
	}

	enum TileType
	{
		FLOOR = 0,
		WALL,
		STAIR,
	};

	// 配置するタイルデータ
	struct TILE_DATA
	{
		TileType type;

		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	// 接続点データ
	struct CONNECTPOINT_DATA
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 angle = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4X4 transform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	};

	virtual void Update(float elapsedTime)
	{
		UpdateTransform();

		for (RoomBase* child : childs)
		{
			child->Update(elapsedTime);
		}
	}

	void UpdateTransform()
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
			if (parentConnectPointIndex > -1)
			{
				DirectX::XMFLOAT4X4 parentTransform = parent->GetConnectPointData(parentConnectPointIndex).transform;
				ParentTransform = DirectX::XMLoadFloat4x4(&parentTransform);
			}
			else
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

	// 自分の深度を取得する
	int GetDepth(int i = 0)
	{
		if (parent == nullptr)
		{
			return i;
		}
		return parent->GetDepth(++i);
	}

	// 自分の部屋タイプを取得する
	DungeonData::RoomType GetRoomType() { return roomType; }

	// 自分を含め全ての部屋を取得する
	std::vector<RoomBase*> GetAll(std::vector<RoomBase*> rooms = {})
	{
		rooms.emplace_back(this);

		for (RoomBase* child : this->childs)
		{
			rooms = child->GetAllChilds(rooms);
		}
		return rooms;
	}

	// 全ての子を取得する
	std::vector<RoomBase*> GetAllChilds(std::vector<RoomBase*> childs = {})
	{
		// 親の数が０なら根なので除外
		if (this->parent != nullptr)
		{
			childs.emplace_back(this);
		}

		for (RoomBase* child : this->childs)
		{
			childs = child->GetAllChilds(childs);
		}
		return childs;
	}

	// 末端の子を取得する
	std::vector<RoomBase*> GetEndChilds(std::vector<RoomBase*> childs = {})
	{
		// 子の数が０なら末端
		if (this->childs.size() == 0)
		{
			childs.emplace_back(this);
		}
		else
		{
			for (RoomBase* child : this->childs)
			{
				childs = child->GetEndChilds(childs);
			}
		}
		return childs;
	}

	// 一番遠い子を取得する
	std::vector<RoomBase*> GetFarthestChild()
	{
		std::vector<RoomBase*> farthestChilds;

		// 末端の子を取得する
		std::vector<RoomBase*> endChilds = GetEndChilds();

		int maxDepth = -1;
		int depth;

		// 末端の子の中から
		for (RoomBase* child : endChilds)
		{
			depth = child->GetDepth();

			if (depth >= maxDepth)
			{
				// 一番遠い子だけを配列に保存する
				maxDepth = depth;
				farthestChilds.emplace_back(child);
			}
		}

		return farthestChilds;
	}

	// 親を設定
	void SetParent(RoomBase* parent) { this->parent = parent; }

	// 子を追加する
	void AddRoom(RoomBase* room) { this->childs.emplace_back(room); }

	// 部屋の接続点データを取得
	std::vector<CONNECTPOINT_DATA> GetConnectPointData() const { return m_connectPointDatas; }
	CONNECTPOINT_DATA GetConnectPointData(int index) const { return m_connectPointDatas.at(index); }

	// 行列取得
	DirectX::XMFLOAT4X4 GetTransform() { return m_transform; }

	// AABB取得
	AABB GetAABB() const { return m_aabb; }

	// 部屋タイルデータをロード
	virtual void LoadMapTileData() {}

	// 部屋タイルを配置
	virtual void PlaceMapTile() {}

	// 出口を配置
	virtual void PlaceExit() {}

	// GUI
	virtual int DrawDebugGUI(int i = 0)
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
			DirectX::XMFLOAT3 debugAngle;
			debugAngle.x = DirectX::XMConvertToDegrees(m_angle.x);
			debugAngle.y = DirectX::XMConvertToDegrees(m_angle.y);
			debugAngle.z = DirectX::XMConvertToDegrees(m_angle.z);
			ImGui::DragFloat3("Angle", &debugAngle.x);
			m_angle.x = DirectX::XMConvertToRadians(debugAngle.x);
			m_angle.y = DirectX::XMConvertToRadians(debugAngle.y);
			m_angle.z = DirectX::XMConvertToRadians(debugAngle.z);
			ImGui::DragFloat3("AABB.pos", &m_aabb.position.x);
			ImGui::DragFloat3("AABB.radii", &m_aabb.radii.x);

			//ImGui::Text(("MapTileSize: " + std::to_string(mapTiles.size())).c_str());
			ImGui::Text(("ParentConnectPointIndex: " + std::to_string(parentConnectPointIndex)).c_str());
			ImGui::Text(("Depth: " + std::to_string(depth)).c_str());

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

protected:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_angle = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4X4 m_transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	std::vector<TILE_DATA> m_tileDatas;
	std::vector<CONNECTPOINT_DATA> m_connectPointDatas;

	//std::vector<MapTile*> mapTiles;

	int parentConnectPointIndex = -1;
	int depth = 0;
	AABB m_aabb = {};		// 当たり判定

	DungeonData::RoomType roomType = DungeonData::RoomType::END_ROOM;

	RoomBase* parent = nullptr;		// 親
	std::vector<RoomBase*> childs;	// 子
};