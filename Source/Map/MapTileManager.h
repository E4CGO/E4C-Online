#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/ModelObject.h"
#include "TAKOEngine/Physics/QuadtreeNode.h"

class MapTileManager : public ObjectManager<ModelObject>, public Singleton<MapTileManager>
{
	friend class Singleton<MapTileManager>;
protected:
	MapTileManager() = default;
	~MapTileManager() = default;
public:
	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera = false);
	// �������C�L���X�g
	bool VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// �l���؋�Ԑ���
	void CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth);
	void CreateQuadtree(uint32_t depth = 1);

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

private:
	QuadtreeNodeManager quadtree;
};

#define MAPTILES MapTileManager::Instance()