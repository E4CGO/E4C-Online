#pragma once

#include "Map/DungeonData.h"
#include <vector>

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

	enum TileType
	{
		FLOOR = 0,
		WALL,
		PILLAR,
		STAIR,
		SPAWNER,
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

	virtual void Initialize() {}

	virtual void Update(float elapsedTime)
	{
		UpdateTransform();

		for (RoomBase* child : childs)
		{
			child->Update(elapsedTime);
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
			// DEAD_END（行き止まり）なら親を登録
			if (this->roomType == DungeonData::DEAD_END)
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
	std::vector<CONNECTPOINT_DATA> GetConnectPointData() const { return m_connectPointDatas; }
	CONNECTPOINT_DATA GetConnectPointData(int index) const { return m_connectPointDatas.at(index); }

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

	std::vector<TILE_DATA> m_tileDatas;
	std::vector<CONNECTPOINT_DATA> m_connectPointDatas;

	float tileScale = 4.0f;

	int parentConnectPointIndex = -1;
	int depth = 0;
	AABB m_aabb = {};		// 当たり判定

	DungeonData::RoomType roomType = DungeonData::RoomType::END_ROOM;

	RoomBase* parent = nullptr;		// 親
	std::vector<RoomBase*> childs;	// 子
};