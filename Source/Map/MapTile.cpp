#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling, RoomBase* parent) :
	ModelObject(filename, scaling, ModelObject::RENDER_MODE::DX11, ModelObject::MODEL_TYPE::LHS_TOON)
{
	this->parent = parent;

	Update(0);// モデル更新

	if (strlen(filename) == 0) return;
<<<<<<< HEAD
	SetCollider(Collider::COLLIDER_TYPE::MAP, Collider::COLLIDER_OBJ::OBSTRUCTION);
	//SetCollider(Collider::COLLIDER_TYPE::MODEL);
=======
>>>>>>> 141e07936558e9fe621bfaedf68e3c4df6ed28a1
}

void MapTile::UpdateTransform()
{
	// スケール行列生成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列生成
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// 位置行列生成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMMATRIX LocalTransform = S * R * T;

	DirectX::XMMATRIX ParentTransform;
	if (parent != nullptr)
	{
		DirectX::XMFLOAT4X4 parentTransform = parent->GetTransform();
		ParentTransform = DirectX::XMLoadFloat4x4(&parentTransform);
	}
	else
	{
		ParentTransform = DirectX::XMMatrixIdentity();
	}
	DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentTransform;

	DirectX::XMStoreFloat4x4(&transform, GlobalTransform);
}

void MapTile::Update(float elapsedTime)
{
	for (auto& model : m_pmodels)
	{
		if (model == nullptr) continue;

		// 行列更新
		UpdateTransform();

		// アニメーション更新
		model->UpdateAnimation(elapsedTime * m_animationSpeed);

		// トランスフォーム更新
		model->UpdateTransform(transform);
	}
}