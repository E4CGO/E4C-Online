#pragma once

#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Physics/Collider.h"
#include "Liner8TreeManager.h"

class MapCollider : public Collider
{
public:
	MapCollider(iModel* model);
	~MapCollider();

	bool CollisionVsShpere(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) override;

	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) override;
protected:
	iModel* model;

	struct Polygon
	{
		DirectX::XMFLOAT3 position[3] = {};
		DirectX::XMFLOAT3 normal = {};
		int materialIndex = -1;
	};
	Liner8TreeManager<Polygon> tree;
	void CalcMapArea(
		DirectX::XMFLOAT3& minPos,
		DirectX::XMFLOAT3& maxPos
	);
	void RegisterPolygons();
	bool ShpereVsPolygon(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		Polygon* polygon,
		HitResult& result
	);

	bool RayVsPolygon(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		Polygon* polygon,
		HitResult& result
	);
	bool RayVsPolygon(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		Polygon* polygon,
		HitResult& result
	);

	bool SearchChildren(	// 子空間探索
		int Elem,
		const DirectX::XMVECTOR& start,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		HitResult& result,
		bool& hit
	);
	bool SearchParent(	// 親空間探索
		int Elem,
		const DirectX::XMVECTOR& start,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		HitResult& result,
		bool& hit
	);
};
