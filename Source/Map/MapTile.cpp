#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling, RoomBase* parent) :
	ModelObject(filename, scaling)
{
	this->parent = parent;

	Update(0);// モデル更新
	//SetCollider(Collider::COLLIDER_TYPE::MAP);
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
	UpdateTransform();

	model->UpdateAnimation(elapsedTime * animationSpeed);

	model->UpdateTransform(transform);
}
