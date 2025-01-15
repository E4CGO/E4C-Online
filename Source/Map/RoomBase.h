#pragma once

#include "TAKOEngine/Rendering/DebugRenderer/CubeRenderer.h"
#include "Map/DungeonData.h"
#include <vector>

using namespace ns_RoomData;

// 部屋ベース
class RoomBase
{
public:
	// コンストラクタ（自動生成）
	RoomBase(
		RoomBase* parent, int pointIndex,
		RoomType roomType,
		std::vector<AABB>& roomAABBs,
		bool& isLastRoomGenerated);

	// コンストラクタ（roomOrderに従う）
	RoomBase(
		RoomBase* parent, int pointIndex,
		RoomType roomType,
		std::vector<AABB>& roomAABBs,
		std::vector<uint8_t>& roomOrder, int& orderIndex);

	// コンストラクタ（当たり判定用、すぐ消す）
	RoomBase(
		RoomBase* parent, int pointIndex,
		RoomType roomType);

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
		// AABBの描画
		// radiiは半径なので2倍して直径にしてからSetCubeを行う
		// 自身のAABB
		{
			DirectX::XMFLOAT3 diameter = {
				m_aabb.radii.x * 2.0f,
				m_aabb.radii.y * 2.0f,
				m_aabb.radii.z * 2.0f
			};
			//m_aabbCube->SetCube(m_aabb.position, diameter, { 1.0f, 1.0f, 1.0f, 1.0f });
			//m_aabbCube->Render(rc);
		}

		// 子の部屋を生成する時のAABB
		for (int i = 0; i < m_debugAABBs.size(); i++)
		{
			DirectX::XMFLOAT3 diameter = {
				m_debugAABBs.at(i).radii.x * 2.0f,
				m_debugAABBs.at(i).radii.y * 2.0f,
				m_debugAABBs.at(i).radii.z * 2.0f
			};
			m_debugCubes.at(i)->SetCube(m_debugAABBs.at(i).position, diameter, { 1.0f, 0.0f, 0.0f, 1.0f });
			m_debugCubes.at(i)->Render(rc);
		}
	}

	void UpdateTransform();

	DirectX::XMFLOAT3 GetPosition() { return m_position; }
	DirectX::XMFLOAT3 GetCenterPos();
	DirectX::XMFLOAT3 GetAngle() { return m_angle; }
	DirectX::XMFLOAT3 GetScale() { return m_scale; }

	// 次の部屋を生成する（自動生成）
	void GenerateNextRoomAutomatically(
		std::vector<AABB>& roomAABBs,
		bool& isLastRoomGenerated);

	// 次の部屋を生成する（roomOrderに従う）
	void GenerateNextRoomFromOrder(
		std::vector<AABB>& roomAABBs,
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
	virtual void LoadMapData();

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

	std::unique_ptr<CubeRenderer> m_aabbCube;
	std::vector<std::unique_ptr<CubeRenderer>> m_debugCubes;

	//std::unique_ptr<CubeRenderer> m_debugCube;
	//std::unique_ptr<CubeRenderer> m_debugDebugCube;
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