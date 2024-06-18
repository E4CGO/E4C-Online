#include <typeinfo>
#include "Collider.h"
#include "ModelCollider.h"

bool Collider::Collision(
	Collider*& other,
	DirectX::XMFLOAT3 direction,
	HitResult& result
)
{
	if (!enable || !other->IsEnable()) return false;

	switch (other->type)
	{
	case Collider::COLLIDER_TYPE::UNROTATED_BOX:
		return CollisionVsUnrotatedBox(other, direction, result);
		break;
	case Collider::COLLIDER_TYPE::MODEL:
		return CollisionVsModel(other, direction, result);
		break;
	case Collider::COLLIDER_TYPE::SPHERE:
		return CollisionVsShpere(other, direction, result);
		break;
	default:
		return false;
		break;
	}
}