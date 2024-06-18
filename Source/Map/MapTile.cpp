#include "MapTile.h"

MapTile::MapTile(const char* filename, float scaling) : ModelObject(filename, scaling)
{
	SetCollider(Collider::COLLIDER_TYPE::MODEL);
}