#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling) : ModelObject(filename, scaling)
{
	Update(0);// ���f���X�V
	SetCollider(Collider::COLLIDER_TYPE::MAP);
}