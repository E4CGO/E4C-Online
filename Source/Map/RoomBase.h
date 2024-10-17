#pragma once

#include <vector>

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

// �����x�[�X
class RoomBase
{
public:
	// �R���X�g���N�^
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
		// �X�P�[���s�񐶐�
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		// ��]�s�񐶐�
		DirectX::XMMATRIX R = AnglesToMatrix(m_angle);
		// �ʒu�s�񐶐�
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		DirectX::XMMATRIX W = S * R * T;

		DirectX::XMStoreFloat4x4(&m_transform, W);
	}

	// �����̐[�x���擾����
	int GetDepth(int i = 0)
	{
		if (parent == nullptr)
		{
			return i;
		}
		return parent->GetDepth(++i);
	}

	// �q��ǉ�����
	void AddRoom(RoomBase* room) { this->child.emplace_back(room); }

	// �����̎n�_���擾
	DirectX::XMFLOAT3 GetStartPos() { return m_startPos; }

	// �����̏I�_���擾
	DirectX::XMFLOAT3 GetEndPos() { return m_endPos; }

	// �����̐ڑ��_���擾
	DirectX::XMFLOAT3 GetNextPos() { return m_nextPos; }

	// �s��擾
	DirectX::XMFLOAT4X4 GetTransform() { return m_transform; }

	// �����^�C����z�u
	virtual void PlaceMapTile() {}

protected:
	DirectX::XMFLOAT3 m_startPos = { 0.0f, 0.0f, 0.0f };	// �����̎n�_
	DirectX::XMFLOAT3 m_endPos = { 0.0f, 0.0f, 0.0f };		// �����̏I�_
	DirectX::XMFLOAT3 m_nextPos = { 0.0f, 0.0f, 0.0f };		// ���̕����̎n�_

	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_angle = { 90.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4X4 m_transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	RoomBase* parent;				// �e
	std::vector<RoomBase*> child;	// �q
};