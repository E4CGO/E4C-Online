#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling) : ModelObject(filename, scaling)
{
	Update(0);// ���f���X�V
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
//	// �s��X�V
//	UpdateTransform(parent->GetTransform());
//
//	// �A�j���[�V�����X�V
//	model->UpdateAnimation(elapsedTime * animationSpeed);
//
//	// �g�����X�t�H�[���X�V
//	model->UpdateTransform(transform);
//}