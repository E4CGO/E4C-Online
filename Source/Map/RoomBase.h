﻿#pragma once

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
			DirectX::XMMATRIX ParentTransform;
			if (parentConnectPointIndex > -1)
			{
				DirectX::XMFLOAT4X4 parentTransform = parent->GetConnectPointData(parentConnectPointIndex).transform;
				//DirectX::XMFLOAT4X4 parentTransform = parent->GetNextPosTranform();
				ParentTransform = DirectX::XMLoadFloat4x4(&parentTransform);
			}
			else
			{
				ParentTransform = DirectX::XMMatrixIdentity();
			}
			DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;
			DirectX::XMStoreFloat4x4(&data.transform, GlobalTransform);

			int a = 0;
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

	// 子を追加する
	void AddRoom(RoomBase* room) { this->childs.emplace_back(room); }

	// 部屋の接続点の数を取得
	int GetConnectPointCount() { return m_connectPointDatas.size(); }

	// 部屋の接続点データを取得
	CONNECTPOINT_DATA GetConnectPointData(int index) { return m_connectPointDatas.at(index); }

	// 行列取得
	DirectX::XMFLOAT4X4 GetTransform() { return m_transform; }

	// 部屋タイルを配置
	virtual void PlaceMapTile() {}

	// 出口を配置
	virtual void PlaceExit() {}

	// GUI
	virtual int DrawDebugGUI(int i = 0) { return i; }

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

	DirectX::XMFLOAT2 sides[4];	// 当たり判定用の辺

	std::vector<TILE_DATA> m_tileDatas;
	std::vector<CONNECTPOINT_DATA> m_connectPointDatas;
	std::vector<DungeonData::RoomType> m_connectableRooms;

	int parentConnectPointIndex = -1;
	int depth = 0;

	DungeonData::RoomType roomType = DungeonData::RoomType::END_ROOM;

	RoomBase* parent = nullptr;		// 親
	std::vector<RoomBase*> childs;	// 子
};