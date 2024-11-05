#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "TAKOEngine/Physics/QuadtreeNode.h"
#include "TAKOEngine/Physics/OctreeNode.h"
#include "TAKOEngine/Physics/Liner8TreeManager.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() { tree.Finalize(); }
public:
	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, HitResult& hit, bool camera = false);
	// �������C�L���X�g
	bool VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	
	// �I�u�W�F�N�g�̃R���_�[�������߂�
	bool IntersectColliderVsMap(Collider* collider);

	// ���̉����߂�
	bool IntersectSphereVsMap(const DirectX::XMVECTOR& spherePos, float radius);

	// ��Ԑ���
	void CreateSpatialIndex(uint32_t quadDepth = 1, uint32_t octDepth = 1, DirectX::XMFLOAT3* minPos = nullptr, DirectX::XMFLOAT3* maxPos = nullptr);

	// �l���؋�Ԃ֎O�p�`�I�u�W�F�N�g��o�^
	bool InsertTriangleObject(Triangle& triangle) { quadtree.InsertTriangleObject(triangle); }
	// �l���؋�Ԃ֋��̃I�u�W�F�N�g��o�^
	bool InsertSphereObject(Sphere& sphere) { quadtree.InsertSphereObject(sphere); }
	// �l���؋�Ԃ�AABB�I�u�W�F�N�g��o�^
	bool InsertAABBObject(AABB& aabb) { quadtree.InsertAABBObject(aabb); }
	// �l���؋�ԂփJ�v�Z���I�u�W�F�N�g��o�^
	bool InsertCapsuleObject(Capsule& capsule) { quadtree.InsertCapsuleObject(capsule); }

protected:
	// �}�b�v�T�C�Y�v�Z
	void CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos);

	// �}�b�v�̃��b�V����o�^(�Ԃ�l�̓��b�V���̎O�p�`�̐�)
	int InsertMapMesh();

	bool SearchChildren(	// �q��ԒT��
		int Elem,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& direction,
		float dist, 
		HitResult& result,
		bool& hit);
	bool SearchParent(	// �e��ԒT��
		int Elem,
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& direction,
		float dist,
		HitResult& result,
		bool& hit);

private:
	QuadtreeNodeManager quadtree;
	OctreeNodeManager octree;
	Liner8TreeManager<Triangle> tree;
};

#define MAPTILES MapTileManager::Instance()