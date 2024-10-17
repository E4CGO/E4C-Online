#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling) : ModelObject(filename, scaling)
{
	Update(0);// モデル更新
	//SetCollider(Collider::COLLIDER_TYPE::MAP);
}

//MapTile::MapTile(const char* filename, RoomBase* parent, float scaling) : ModelObject(filename, scaling)
//{
//	this->parent = parent;
//
//	Update(0);
//}

//MapTile::~MapTile()
//{
//	if (parent != nullptr) delete parent;
//}
//
//void MapTile::Update(float elapsedTime)
//{
//	// 行列更新
//	UpdateTransform(parent->GetTransform());
//
//	// アニメーション更新
//	model->UpdateAnimation(elapsedTime * animationSpeed);
//
//	// トランスフォーム更新
//	model->UpdateTransform(transform);
//}