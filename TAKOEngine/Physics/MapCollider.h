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

	//void CalcMapArea(
	//	DirectX::XMFLOAT3& minPos,
	//	DirectX::XMFLOAT3& maxPos
	//);
protected:
	iModel* model;

	//struct Porigon
	//{
	//	DirectX::XMFLOAT3 position[3] = {};
	//	DirectX::XMFLOAT3 normal = {};
	//	int materialIndex = -1;
	//};
	//Liner8TreeManager<Porigon> tree;
	//void RegisterPorigons();
	//bool ShpereVsPorigon(
	//	Collider*& other,
	//	DirectX::XMFLOAT3& direction,
	//	Porigon* porigon,
	//	HitResult& result
	//);
	//bool RayVsPorigon(
	//	const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end,
	//	Porigon* porigon,
	//	HitResult& result
	//);
	//bool SearchChildren(	// 子空間探索
	//	int Elem,
	//	const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end,
	//	HitResult& result,
	//	bool& hit
	//);
	//bool SearchChildren(	// 子空間探索
	//	
	//	int Elem,
	//	bool& hit
	//);
	//bool SearchParent(	// 親空間探索
	//	int Elem,
	//	const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end,
	//	HitResult& result,
	//	bool& hit
	//);
};
