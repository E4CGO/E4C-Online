#include "MapTileManager.h"

// レイキャスト
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	if (start == end) return false;

	HitResult temp;
	hit.distance = FLT_MAX;
	for (ModelObject*& item : items)
	{
		if (item->GetCollider() == nullptr) continue;
		if (item->GetCollider()->RayCast(start, end, temp) && (temp.distance < hit.distance))
		{
			hit = temp; // Å’Z
		}
	}
	return hit.distance < FLT_MAX;
}
