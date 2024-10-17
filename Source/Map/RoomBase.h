#pragma once

#include <vector>

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

// 部屋ベース
class RoomBase
{
public:
	// コンストラクタ
	RoomBase(RoomBase* parent, DirectX::XMFLOAT3 moveValue = { 0.0f, 0.0f, 0.0f })
	{
		this->parent = parent;

		m_startPos += moveValue;
		m_endPos += moveValue;
		m_nextPos += moveValue;

		PlaceMapTile();
	}

	virtual ~RoomBase()
	{
		for (RoomBase* room : child)
		{
			if (room != nullptr) delete room;
		}
	}

	struct TILE_DATA
	{
		DirectX::XMFLOAT3 position;
	};

	virtual void Update(float elapsedTime)
	{
		UpdateTransform();
	}

	void UpdateTransform()
	{
		// スケール行列生成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		// 回転行列生成
		DirectX::XMMATRIX R = AnglesToMatrix(m_angle);
		// 位置行列生成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		DirectX::XMMATRIX W = S * R * T;

		DirectX::XMStoreFloat4x4(&m_transform, W);
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

	// 子を追加する
	void AddRoom(RoomBase* room) { this->child.emplace_back(room); }

	// 部屋の始点を取得
	DirectX::XMFLOAT3 GetStartPos() { return m_startPos; }

	// 部屋の終点を取得
	DirectX::XMFLOAT3 GetEndPos() { return m_endPos; }

	// 部屋の接続点を取得
	DirectX::XMFLOAT3 GetNextPos() { return m_nextPos; }

	// 行列取得
	DirectX::XMFLOAT4X4 GetTransform() { return m_transform; }

	// 部屋タイルを配置
	virtual void PlaceMapTile() {}

protected:
	DirectX::XMFLOAT3 m_startPos = { 0.0f, 0.0f, 0.0f };	// 部屋の始点
	DirectX::XMFLOAT3 m_endPos = { 0.0f, 0.0f, 0.0f };		// 部屋の終点
	DirectX::XMFLOAT3 m_nextPos = { 0.0f, 0.0f, 0.0f };		// 次の部屋の始点

	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_angle = { 90.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4X4 m_transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	RoomBase* parent;				// 親
	std::vector<RoomBase*> child;	// 子
};