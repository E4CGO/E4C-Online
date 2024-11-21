//! @file QuadtreeNode.h
//! @note �l���؋��

#ifndef __QUADTREE_NODE_H__
#define __QUADTREE_NODE_H__

#include "TAKOEngine/Physics/Collision.h"

/**************************************************************************//**
	@class		QuadtreeNode
	@brief		�l���؋�Ԃ̈�̃m�[�h
	@par		���̋�Ԃɑ��݂���I�u�W�F�N�g��o�^����
*//***************************************************************************/
class QuadtreeNode
{
public:
	QuadtreeNode() {}
	QuadtreeNode(DirectX::XMFLOAT3 _center, float _halfSize) { this->m_center = _center; this->m_halfSize = _halfSize; }
	~QuadtreeNode() {}

public:
	DirectX::XMFLOAT3 GetCenter() const { return m_center; }
	void SetCenter(DirectX::XMFLOAT3 _center) { this->m_center = _center; }
	float GetHalfSize() const { return m_halfSize; }
	void SetHalfSize(float _halfSize) { this->m_halfSize = _halfSize; }

	std::vector<Triangle> GetTriangles() const { return m_triangles; }
	std::vector<Sphere*> GetSpheres() const { return m_spheres; }
	std::vector<AABB*> GetAABBs() const { return m_aabbs; }
	std::vector<Capsule*> GetCapsules() const { return m_capsules; }

	// �I�u�W�F�N�g��o�^
	void InsertTriangleObject(Triangle& triangle) { m_triangles.push_back(triangle); }
	void InsertSphereObject(Sphere& sphere) { m_spheres.emplace_back(&sphere); }
	void InsertAABBObject(AABB& aabb) { m_aabbs.emplace_back(&aabb); }
	void InsertCapsuleObject(Capsule& capsule) { m_capsules.emplace_back(&capsule); }
	// �I�u�W�F�N�g���폜
	void ClearTriangleObject() { m_triangles.clear(); }
	void ClearSphereObject() { m_spheres.clear(); }
	void ClearAABBObject() { m_aabbs.clear(); }
	void ClearCapsuleObject() { m_capsules.clear(); }

private:
	DirectX::XMFLOAT3		m_center = {};		// �l���؋�Ԃ̒��S
	float					m_halfSize = {};	// �l���؋�Ԃ̔��Ӓ�
	// �o�^����I�u�W�F�N�g���Ǘ�����}�l�[�W���[
	std::vector<Triangle>		m_triangles = {};		// �O�p�`�o�^�p
	std::vector<Sphere*>		m_spheres = {};			// ���o�^�p
	std::vector<AABB*>			m_aabbs = {};			// AABB�o�^�p
	std::vector<Capsule*>		m_capsules = {};		// �J�v�Z���o�^�p
};


/**************************************************************************//**
	@class		QuadtreeNodeManager
	@brief		�l���؋�ԃ}�l�[�W���[
	@par		�l���؋�ԃm�[�h����`��ɕ��ׁA�l�X�ȏ���������s��
*//***************************************************************************/
class QuadtreeNodeManager
{
public:
	QuadtreeNodeManager()
	{
		m_depth = 0;
	}
	~QuadtreeNodeManager() {}

public:
	struct CrossedNode	// ���������m�[�h�̋L�^�p�\����
	{
		int			m_linerIndex = 0;	// �z��ԍ�
		int			m_level = 0;		// �K�w�ԍ�
	};

	// ���`�l���؂̊K�w�̃X�^�[�g�C���f�b�N�X�̎擾
	const uint32_t GetLevelStart(uint32_t level)
	{
		return LevelStart[level];
	}
	// ���`�l���؂̃C���f�b�N�X����K�w���擾
	const uint32_t GetLevelFromIndex(uint32_t idx)
	{
		uint32_t ret = MAX_DEPTH;
		while (ret >= 0)
		{
			if (LevelStart[--ret] <= idx) break;
		}
		return ret;
	}

	// �l���؋�Ԃ̐���
	void CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth);

	// ���[�g���R�[�h�̐���
	uint32_t GetMortonCode(const DirectX::XMFLOAT3& point, const QuadtreeNode& route, float halfSize);
	uint32_t bitSeparete(uint32_t n);

	// ���K�w�ׂ̗̃��[�g���R�[�h���擾
	int GetNextMortonCode(uint32_t before, uint32_t shiftXYZ, bool minus);

	// �l���؋�ԂփI�u�W�F�N�g��o�^����ۂ̐��`�C���f�b�N�X���擾
	uint32_t GetLinerIndexInsertObject(DirectX::XMFLOAT3 minPoint, DirectX::XMFLOAT3 maxPoint);

	// �l���؋�Ԃ֎O�p�`�I�u�W�F�N�g��o�^
	bool InsertTriangleObject(Triangle& triangle);
	// �l���؋�Ԃ֋��̃I�u�W�F�N�g��o�^
	bool InsertSphereObject(Sphere& sphere);
	// �l���؋�Ԃ�AABB�I�u�W�F�N�g��o�^
	bool InsertAABBObject(AABB& aabb);
	// �l���؋�ԂփJ�v�Z���I�u�W�F�N�g��o�^
	bool InsertCapsuleObject(Capsule& capsule);

	// �I�u�W�F�N�g���l���؋�Ԃ���폜
	void ClearAllTriangleObject();
	void ClearAllSphereObject();
	void ClearAllAABBObject();
	void ClearAllCapsuleObject();

	// �S�Ẵm�[�h�ŎO�p�`�Ɛ������C�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	bool IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result);
	// �S�Ẵm�[�h�ŎO�p�`�ƃ��C�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	bool IntersectRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result);

	// �S�Ẵm�[�h�ŎO�p�`�ƃX�t�B�A�L���X�g�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	bool IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result);
	bool IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result, std::vector<CrossedNode>* nodeList);

	// �S�ẴI�u�W�F�N�g�̉����߂����������s
	void CollisionAllObjects(
		uint32_t target,
		bool singleNode,
		std::vector<Sphere*>* upperListSphere = {},
		std::vector<Capsule*>* upperListCapsule = {},
		std::vector<AABB*>* upperListAABB = {},
		std::vector<Triangle>* upperListTriangle = {});

	// �w�肵���m�[�h�ȉ��S�Ẵm�[�h�œ����I�u�W�F�N�g���m�̉����߂����������s
	void CollisionSphereVsSphere(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere);
	void CollisionCapsuleVsCapsule(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule);
	void CollisionAABBVsAABB(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB);

	// �w�肵���m�[�h�ȉ��S�Ẵm�[�h�ŃI�u�W�F�N�g�ƎO�p�`�̉����߂����������s�@���O�p�`���͓������Ȃ�
	void CollisionSphereVsTriangle(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Triangle>* upperListTriangle);
	void CollisionCapsuleVsTriangle(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<Triangle>* upperListTriangle);
	void CollisionAABBVsTriangle(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB, std::vector<Triangle>* upperListTriangle);

	// �w�肵���m�[�h�ȉ��S�Ẵm�[�h�ŃI�u�W�F�N�g�ƃI�u�W�F�N�g�̉����߂����������s
	void CollisionSphereVsCapsule(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Capsule*>* upperListCapsule);
	void CollisionSphereVsAABB(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<AABB*>* upperListAABB);
	void CollisionCapsuleVsAABB(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<AABB*>* upperListAABB);

	private:
		std::vector<QuadtreeNode>	m_quadtreeNodes = {};	// �l���؃m�[�h�z��
		uint32_t	m_depth = 0;	// ������

		static const uint32_t	MAX_DEPTH = 10u;	// �ő啪����
		static constexpr uint32_t LevelStart[MAX_DEPTH] =	// ���`�l���؂̊e�K�w�̊J�n�C���f�b�N�X�B���䐔��̘a�̌����𗘗p���邱�Ƃŋ��܂邪�萔�Ȃ̂ł��炩���ߌv�Z���Ă���
		{
			0, 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381
		};
};
#endif // !__QUADTREE_NODE_H__