#pragma once

#include "TAKOEngine/Rendering/DebugRenderer/CubeRenderer.h"
#include "Map/DungeonData.h"
#include <vector>

using namespace ns_RoomData;

// 部屋ベース
class RoomBase
{
public:
	// コンストラクタ
	RoomBase(
		RoomBase* parent, int pointIndex,
		std::vector<AABB>& roomAABBs,
		bool isAutoGeneration,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

	// ですとら
	virtual ~RoomBase()
	{
		for (RoomBase* room : childs)
		{
			if (room != nullptr) delete room;
		}
	}

	virtual void Initialize() {}

	virtual void Update(float elapsedTime)
	{
		UpdateTransform();

		for (RoomBase* child : childs)
		{
			child->Update(elapsedTime);
		}
	}

	virtual void Render(const RenderContextDX12 rc)
	{
		{
			DirectX::XMFLOAT3 aabbDrawPos;
			aabbDrawPos = {
				m_aabb.position.x - (m_aabb.radii.x * 0.5f),
				m_aabb.position.y - (m_aabb.radii.y * 0.5f),
				m_aabb.position.z - (m_aabb.radii.z * 0.5f),
			};
			m_debugCube->SetCube(aabbDrawPos, m_aabb.radii, { 1.0f, 1.0f, 1.0f, 1.0f });
			//m_debugCube->Render(rc);
		}

		for (const AABB& d_aabb : m_debugAABBs)
		{
			DirectX::XMFLOAT3 d_aabbPos;
			d_aabbPos = {
				d_aabb.position.x - (d_aabb.radii.x * 0.5f),
				d_aabb.position.y - (d_aabb.radii.y * 0.5f),
				d_aabb.position.z - (d_aabb.radii.z * 0.5f),
			};
			m_debugDebugCube->SetCube(d_aabbPos, d_aabb.radii, { 1.0f, 0.0f, 0.0f, 1.0f });
			//m_debugDebugCube->Render(rc);
		}
	}

	void UpdateTransform();

	DirectX::XMFLOAT3 GetPosition() { return m_position; }
	DirectX::XMFLOAT3 GetCenterPos();
	DirectX::XMFLOAT3 GetAngle() { return m_angle; }
	DirectX::XMFLOAT3 GetScale() { return m_scale; }

	// 次の部屋を生成する
	void GenerateNextRoom(
		std::vector<AABB>& roomAABBs,
		bool isAutoGeneration,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

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
	RoomType GetRoomType() { return roomType; }

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
			// DEAD_END（行き止まり）なら親を登録
			if (this->roomType == RoomType::DEAD_END)
			{
				childs.emplace_back(this->parent);
			}
			// それ以外なら自分を登録
			else
			{
				childs.emplace_back(this);
			}
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
	std::vector<TILE_DATA> GetConnectPointData() const { return m_connectPointDatas; }
	const TILE_DATA GetConnectPointData(int index) const { return m_connectPointDatas.at(index); }

	// 行列取得
	DirectX::XMFLOAT4X4 GetTransform() { return m_transform; }

	// AABB取得
	AABB GetAABB() const { return m_aabb; }
	// AABB算出
	AABB CalcAABB(AABB aabb, DirectX::XMFLOAT3 pos, float degree) const;

	// 部屋データをロード
	virtual void LoadMapData() {}

	// 部屋タイルを配置
	void PlaceMapTile(bool isLeader = false);

	// 出口を配置
	virtual void PlaceExit() {}

	// GUI
	virtual int DrawDebugGUI(int i = 0);

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

	std::vector<std::vector<TILE_DATA>> m_tileDatas;

	std::unique_ptr<CubeRenderer> m_debugCube;

	std::unique_ptr<CubeRenderer> m_debugDebugCube;
	std::vector<AABB> m_debugAABBs;

	//std::vector<TILE_DATA> m_tileDatas;
	std::vector<TILE_DATA> m_connectPointDatas;

	float tileScale = 4.0f;

	int parentConnectPointIndex = -1;
	int depth = 0;
	AABB m_aabb = {};		// 当たり判定

	RoomType roomType = RoomType::END_ROOM;

	RoomBase* parent = nullptr;		// 親
	std::vector<RoomBase*> childs;	// 子
};