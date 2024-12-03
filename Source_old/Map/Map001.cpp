#include "Map001.h"

#include "TAKOEngine/Physics/Collider/ModelCollider.h"

Map001::Map001() : MapTile("Data/Model/Map/Map001.glb")
{
	mapCollider = std::make_unique<ModelDX11>(T_GRAPHICS.GetDevice(), "Data/Model/Map/Map001.collider.glb");
	this->collider = std::make_unique<ModelCollider>(mapCollider.get(), Collider::COLLIDER_OBJ::OBSTRUCTION, &transform);
}

void Map001::Update(float elapsedTime)
{
	ModelObject::Update(elapsedTime);
	mapCollider->UpdateTransform(transform);
}