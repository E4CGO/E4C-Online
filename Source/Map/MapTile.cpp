#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling) : ModelObject(filename, scaling)
{
	Update(0);// モデル更新
	SetCollider(Collider::COLLIDER_TYPE::MAP);
}