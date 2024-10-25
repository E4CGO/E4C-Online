//! @file QuadtreeNode.cpp
//! @note �l���؋��
#include "TAKOEngine/Physics/QuadtreeNode.h"

/**************************************************************************//**
	@brief		�l���؋�Ԃ̐��� �C���f�b�N�X�̑��������͂������̏���
	@param[in]	XMFLOAT3 center : ���[�g��Ԃ̒��S���W
				float halfSize : ���[�g��Ԃ̔��Ӓ�
				uint32_t depth : ��ԕ�����
	@return		�Ȃ�
*//***************************************************************************/
void QuadtreeNodeManager::CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth)
{
	// �ő�K�w���`�F�b�N
	if (depth > MAX_DEPTH)
	{
		depth = MAX_DEPTH;
	}

	// �K�w���ۑ�
	this->m_depth = depth;

	// ���[�g��Ԃ�o�^
	QuadtreeNode tmp(center, halfSize);
	m_quadtreeNodes.push_back(tmp);

	// ���[�g�ȉ��̑S�Ẵm�[�h���쐬
	for (uint32_t level = 1; level <= depth; level++)
	{
		float nodeHalfSize = halfSize / (1 << level);
		tmp.SetHalfSize(nodeHalfSize);

		for (uint32_t linerIndex = GetLevelStart(level - 1); linerIndex < GetLevelStart(level); linerIndex++)
		{
			tmp.SetCenter({ m_quadtreeNodes[linerIndex].GetCenter().x - nodeHalfSize, 0, m_quadtreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_quadtreeNodes.push_back(tmp);

			tmp.SetCenter({ m_quadtreeNodes[linerIndex].GetCenter().x + nodeHalfSize, 0, m_quadtreeNodes[linerIndex].GetCenter().z - nodeHalfSize });
			m_quadtreeNodes.push_back(tmp);

			tmp.SetCenter({ m_quadtreeNodes[linerIndex].GetCenter().x - nodeHalfSize, 0, m_quadtreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_quadtreeNodes.push_back(tmp);

			tmp.SetCenter({ m_quadtreeNodes[linerIndex].GetCenter().x + nodeHalfSize, 0, m_quadtreeNodes[linerIndex].GetCenter().z + nodeHalfSize });
			m_quadtreeNodes.push_back(tmp);
		}
	}
}

/**************************************************************************//**
	@brief		���[�g���R�[�h(����̊K�w�̋�Ԕԍ�)�̐���
	@param[in]	XMFLOAT3 point : ���[�g���R�[�h�����߂������W
				QuadtreeNode route : ���[�g���
				float halfSize : ���̊K�w�̔��Ӓ�
	@return		uint32_t
*//***************************************************************************/
uint32_t QuadtreeNodeManager::GetMortonCode(const DirectX::XMFLOAT3& point, const QuadtreeNode& route, float halfSize)
{
	float x = (point.x - (route.GetCenter().x - route.GetHalfSize())) / (2 * halfSize);
	float z = (point.z - (route.GetCenter().z - route.GetHalfSize())) / (2 * halfSize);
	return bitSeparete(static_cast<uint32_t>(x)) + (bitSeparete(static_cast<uint32_t>(z)) << 1);
}
uint32_t QuadtreeNodeManager::bitSeparete(uint32_t n)
{
	n = (n ^ (n << 8)) & 0x00ff00ff;
	n = (n ^ (n << 4)) & 0x0f0f0f0f;
	n = (n ^ (n << 2)) & 0x33333333;
	n = (n ^ (n << 1)) & 0x55555555;
	return n;
}

/**************************************************************************//**
	@brief		���K�w�ׂ̗̃��[�g���R�[�h���擾
	@param[in]	uint32_t before : ���̃��[�g���R�[�h�i����`���̊e�K�w�̃��[�g���R�[�h�j
				uint32_t shiftXZ : �ǂ̎��ׂ̗̃��[�g���R�[�h���~�������w��@x=0, z=1
				bool minus : �}�C�i�X�����ɐi�ޏꍇ��ture
	@return		int
*//***************************************************************************/
int QuadtreeNodeManager::GetNextMortonCode(uint32_t before, uint32_t shiftXZ, bool minus)
{
	int ret = before;

	uint32_t filter = 0x01 << shiftXZ;
	uint32_t value = 1 << shiftXZ;

	// before��filter���傫���ԃ��[�v����
	while (before >= filter)
	{
		// �t�B���^�[�������Ēl���`�F�b�N���Abreak����
		if ((!(before & filter) && (!minus)) || ((before & filter) && minus))	break;

		// before��2bit�E�V�t�g�����čă`�F�b�N�B�����ă��[�g���R�[�h�̍X�V�p�̍����l��value���X�V����
		before >>= 2;
		value = value * 4 - filter;
	}

	if (minus)
	{
		ret -= value;
	}
	else
	{
		ret += value;
	}

	return ret;
}

/**************************************************************************//**
	@brief		�l���؋�ԂփI�u�W�F�N�g��o�^����ۂ̐��`�C���f�b�N�X���擾
	@param[in]	XMFLOAT3 minPoint : AABB�̍ŏ��_
				XMFLOAT3 maxPoint : AABB�̍ő�_
	@return		uint32_t
*//***************************************************************************/
uint32_t QuadtreeNodeManager::GetLinerIndexInsertObject(DirectX::XMFLOAT3 minPoint, DirectX::XMFLOAT3 maxPoint)
{
	// AABB�̍ŏ��_�̃��[�g���R�[�h
	uint32_t mortonCodeMin = GetMortonCode(minPoint, m_quadtreeNodes.front(), m_quadtreeNodes.back().GetHalfSize());
	// AABB�̍ő�_�̃��[�g���R�[�h
	uint32_t mortonCodeMax = GetMortonCode(maxPoint, m_quadtreeNodes.front(), m_quadtreeNodes.back().GetHalfSize());
	// ��L��̃��[�g���R�[�h�̔r���I�_���a
	uint32_t mortonCodeXOR = mortonCodeMin ^ mortonCodeMax;

	// ���[�g���R�[�h�̔r���I�_���a���g���A��ʋ�Ԃɓo�^���邩�̃`�F�b�N
	uint32_t count = 1;
	uint32_t shift = 0;

	while (mortonCodeXOR != 0)
	{
		// ����2bit���}�X�N���Ēl���`�F�b�N����
		if (mortonCodeXOR & 0x03)
		{
			// �V�t�g�����㏑���ۑ�
			shift = count;
		}
		// �r���I�_���a��2bit�V�t�g�����čă`�F�b�N
		mortonCodeXOR >>= 2;

		count++;
	}

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	return  (mortonCodeMin >> (shift * 2)) + GetLevelStart(m_depth - shift);
}

/**************************************************************************//**
	@brief		�l���؋�Ԃ֎O�p�`�I�u�W�F�N�g��o�^
	@param[in]	Triangle triangle : �o�^����O�p�`
	@return		bool : �o�^������true
*//***************************************************************************/
bool QuadtreeNodeManager::InsertTriangleObject(Triangle& triangle)
{
	// �O�p�`���܂�AABB���\������ŏ��_�ƍő�_���쐬�iy���W�������j
	DirectX::XMFLOAT3 minPoint = triangle.position[0];
	DirectX::XMFLOAT3 maxPoint = triangle.position[0];

	if (minPoint.x > triangle.position[1].x)
	{
		minPoint.x = triangle.position[1].x;
	}
	else if (maxPoint.x < triangle.position[1].x)
	{
		maxPoint.x = triangle.position[1].x;
	}

	if (minPoint.x > triangle.position[2].x)
	{
		minPoint.x = triangle.position[2].x;
	}
	else if (maxPoint.x < triangle.position[2].x)
	{
		maxPoint.x = triangle.position[2].x;
	}

	if (minPoint.z > triangle.position[1].z)
	{
		minPoint.z = triangle.position[1].z;
	}
	else if (maxPoint.z < triangle.position[1].z)
	{
		maxPoint.z = triangle.position[1].z;
	}

	if (minPoint.z > triangle.position[2].z)
	{
		minPoint.z = triangle.position[2].z;
	}
	else if (maxPoint.z < triangle.position[2].z)
	{
		maxPoint.z = triangle.position[2].z;
	}

	// �ŏ��_�E�ő�_���l���؂̃��[�g��Ԃ̊O�ɂ���ꍇ�A�o�^���s
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_quadtreeNodes.front().GetCenter();
	routeMaxPoint.x += m_quadtreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_quadtreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_quadtreeNodes.size()) return false;

	// �o�^
	m_quadtreeNodes[linerIndex].InsertTriangleObject(triangle);

	return true;
}

/**************************************************************************//**
	@brief		�l���؋�Ԃ֋��̃I�u�W�F�N�g��o�^
	@param[in]	Sphere sphere : �o�^���鋅��
	@return		bool : �o�^������true
*//***************************************************************************/
bool QuadtreeNodeManager::InsertSphereObject(Sphere& sphere)
{
	// �����܂�AABB���\������ŏ��_�ƍő�_���쐬
	DirectX::XMFLOAT3 minPoint = {};
	DirectX::XMFLOAT3 maxPoint = {};
	sphere.GetBoundPoints(&minPoint, &maxPoint);

	// �ŏ��_�E�ő�_���l���؂̃��[�g��Ԃ̊O�ɂ���ꍇ�A�o�^���s
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_quadtreeNodes.front().GetCenter();
	routeMaxPoint.x += m_quadtreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_quadtreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_quadtreeNodes.size()) return false;

	// �o�^
	m_quadtreeNodes[linerIndex].InsertSphereObject(sphere);

	return true;
}

/**************************************************************************//**
	@brief		�l���؋�Ԃ�AABB�I�u�W�F�N�g��o�^
	@param[in]	AABB aabb : �o�^����AABB
	@return		bool : �o�^������true
*//***************************************************************************/
bool QuadtreeNodeManager::InsertAABBObject(AABB& aabb)
{
	// AABB���\������ŏ��_�ƍő�_���쐬�iy���W�������j
	DirectX::XMFLOAT3 minPoint = { aabb.position.x - aabb.radii.x, 0.0f, aabb.position.z - aabb.radii.z };
	DirectX::XMFLOAT3 maxPoint = { aabb.position.x + aabb.radii.x, 0.0f, aabb.position.z + aabb.radii.z };

	// �ŏ��_�E�ő�_���l���؂̃��[�g��Ԃ̊O�ɂ���ꍇ�A�o�^���s
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_quadtreeNodes.front().GetCenter();
	routeMaxPoint.x += m_quadtreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_quadtreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_quadtreeNodes.size()) return false;

	// �o�^
	m_quadtreeNodes[linerIndex].InsertAABBObject(aabb);

	return true;
}

/**************************************************************************//**
	@brief		�l���؋�ԂփJ�v�Z���I�u�W�F�N�g��o�^
	@param[in]	Capsule capsule : �o�^����J�v�Z��
	@return		bool : �o�^������true
*//***************************************************************************/
bool QuadtreeNodeManager::InsertCapsuleObject(Capsule& capsule)
{
	// �J�v�Z�����܂�AABB���\������ŏ��_�ƍő�_���쐬�iy���W�������j
	DirectX::XMFLOAT3 minPoint = {};
	DirectX::XMFLOAT3 maxPoint = {};
	capsule.GetBoundPoints(&minPoint, &maxPoint);

	// �ŏ��_�E�ő�_���l���؂̃��[�g��Ԃ̊O�ɂ���ꍇ�A�o�^���s
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();

	DirectX::XMFLOAT3 routeMaxPoint = m_quadtreeNodes.front().GetCenter();
	routeMaxPoint.x += m_quadtreeNodes.front().GetHalfSize();
	routeMaxPoint.z += m_quadtreeNodes.front().GetHalfSize();

	if (minPoint.x < routeMinPoint.x)	return false;
	if (minPoint.z < routeMinPoint.z)	return false;
	if (maxPoint.x > routeMaxPoint.x)	return false;
	if (maxPoint.z > routeMaxPoint.z)	return false;

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	int linerIndex = GetLinerIndexInsertObject(minPoint, maxPoint);
	if (linerIndex >= m_quadtreeNodes.size()) return false;

	// �o�^
	m_quadtreeNodes[linerIndex].InsertCapsuleObject(capsule);

	return true;
}

/**************************************************************************//**
	@brief		�I�u�W�F�N�g���l���؋�Ԃ���폜
	@param[in]	�Ȃ�
	@return		�Ȃ�
*//***************************************************************************/
// �S�Ă̎O�p�`�I�u�W�F�N�g���l���؋�Ԃ���폜
void QuadtreeNodeManager::ClearAllTriangleObject()
{
	for (auto it = m_quadtreeNodes.begin(); it != m_quadtreeNodes.end(); it++)
	{
		it->ClearTriangleObject();
	}
}
// �S�Ă̋��̃I�u�W�F�N�g���l���؋�Ԃ���폜
void QuadtreeNodeManager::ClearAllSphereObject()
{
	for (auto it = m_quadtreeNodes.begin(); it != m_quadtreeNodes.end(); it++)
	{
		it->ClearSphereObject();
	}
}
// �S�Ă�AABB�I�u�W�F�N�g���l���؋�Ԃ���폜
void QuadtreeNodeManager::ClearAllAABBObject()
{
	for (auto it = m_quadtreeNodes.begin(); it != m_quadtreeNodes.end(); it++)
	{
		it->ClearAABBObject();
	}
}
// �S�ẴJ�v�Z���I�u�W�F�N�g���l���؋�Ԃ���폜
void QuadtreeNodeManager::ClearAllCapsuleObject()
{
	for (auto it = m_quadtreeNodes.begin(); it != m_quadtreeNodes.end(); it++)
	{
		it->ClearCapsuleObject();
	}
}

/**************************************************************************//**
	@brief		�S�Ẵm�[�h�ŎO�p�`�Ɛ������C�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	@param[in]	XMFLOAT3 rayStart : ���C�̎n�_
				XMFLOAT3 rayEnd : ���C�̏I�_
				HitResultVector result : �q�b�g���ʋL�^�p�ϐ�
	@return		bool : ��������Ƃ�true
*//***************************************************************************/
bool QuadtreeNodeManager::IntersectVerticalRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result)
{
	bool ret = false;

	// ���C�̌�����p�ӂ��A�����ă��C�̒����Ńq�b�g���̋�����������
	DirectX::XMFLOAT3 rayDirection = { 0.0f, rayEnd.y - rayStart.y , 0.0f };
	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&rayDirection)));
	result.distance = rayDist;

	// ���C�̎n�_�̃��[�g���R�[�h�Ɛ��`�C���f�b�N�X���Z�o
	uint32_t mortonCode = GetMortonCode(rayStart, m_quadtreeNodes.front(), m_quadtreeNodes[GetLevelStart(m_depth)].GetHalfSize());
	int linerIndex = GetLevelStart(m_depth) + mortonCode;

	// �l���؂̊K�w�������[�v����
	for (int level = m_depth; level >= 0; level--)
	{
		QuadtreeNode targetNode = m_quadtreeNodes.at(linerIndex);

		// �e�m�[�h�����O�p�`�S�Ăƃ��C�̓����蔻����s��
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			HitResultVector tmpResult;
			DirectX::XMVECTOR triangleVerts[3] = {
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectRayVsTriangle(DirectX::XMLoadFloat3(&rayStart), DirectX::XMLoadFloat3(&rayDirection), triangleVerts, tmpResult))
			{
				if (result.distance > tmpResult.distance)
				{
					result.distance = tmpResult.distance;
					result.normal = tmpResult.normal;
					result.position = tmpResult.position;
					result.triangleVerts[0] = tmpResult.triangleVerts[0];
					result.triangleVerts[1] = tmpResult.triangleVerts[1];
					result.triangleVerts[2] = tmpResult.triangleVerts[2];
					result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
					ret = true;
				}
			}
		}

		// �e�K�w�ɃC���f�b�N�X���ړ�
		mortonCode >>= 2;
		if (level > 0)
			linerIndex = GetLevelStart(level - 1) + mortonCode;
	}

	return ret;
}

/**************************************************************************//**
	@brief		�S�Ẵm�[�h�ŎO�p�`�ƃ��C�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	@param[in]	XMFLOAT3 rayStart : ���C�̎n�_
				XMFLOAT3 rayEnd : ���C�̏I�_
				HitResultVector result : �q�b�g���ʋL�^�p�ϐ�
	@return		bool : ��������Ƃ�true
*//***************************************************************************/
bool QuadtreeNodeManager::IntersectRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResultVector& result)
{
	// �l���؋�Ԃ̍ŏ��l�ix,z���W�j
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();
	// ����p�ϐ�
	bool ret = false;	// �S�̂̌������ʁB��x�ł��O�p�`�Ƃ̌������o���true
	bool hit = false;	// �K�w���Ƃ̌������ʁB��x�ł��O�p�`�̌������o���true

	// ���C�̌�����p�ӂ��A�����ă��C�̒����Ńq�b�g���̋�����������
	DirectX::XMFLOAT3 rayDirection = { rayEnd.x - rayStart.x, rayEnd.y - rayStart.y, rayEnd.z - rayStart.z };
	//float dirx = rayEnd.x - rayStart.x;
	//float diry = rayEnd.y - rayStart.y;
	//float dirz = rayEnd.z - rayStart.z;

	//DirectX::XMFLOAT3 rayDirection{};

	//rayDirection.x = dirx;
	//rayDirection.y = diry;
	//rayDirection.z = dirz;

	////rayDirection = {
	////	rayEnd.x - rayStart.x,
	////	rayEnd.y - rayStart.y,
	////	rayEnd.z - rayStart.z };

	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&rayDirection)));
	result.distance = rayDist;

	// xz�����ꂼ��̃��C�̌������v���X���P�A�[�����O�A�}�C�i�X���|�P�ŋL�^
	int directionX = rayDirection.x > 0.0f ? 1 : (rayDirection.x < 0.0f ? -1 : 0);
	int directionZ = rayDirection.z > 0.0f ? 1 : (rayDirection.z < 0.0f ? -1 : 0);


	// �K�w�����̃��[�v�����B���[�g��Ԃ���X�^�[�g
	for (uint32_t level = 0; level <= m_depth; level++)
	{
		// ���݂̊K�w�̐��`�C���f�b�N�X���擾
		int levelStart = GetLevelStart(level);

		// ���C�̎n�_�̃��[�g���R�[�h���Z�o
		uint32_t mortonCode = GetMortonCode(rayStart, m_quadtreeNodes.front(), m_quadtreeNodes[levelStart].GetHalfSize());

		// DDA���g���ă��C�ŒH��m�[�h���Z�o���邽�߂̊e�ϐ��̏���

		// ��Ԃ̒����̂�xz���̕ӂ̒���
		float cubeSize = m_quadtreeNodes[levelStart].GetHalfSize() * 2;

		// ���C�̌X�������m�[�h��i�߂����̎��̃m�[�h�܂ł̋����̑����ʂ̒萔�u�����̂�xz���̕ӂ̒���/|���C�̐���|�v���Z�o
		float dx = directionX != 0 ? cubeSize / fabsf(rayDirection.x) : 0.0f;
		float dz = directionZ != 0 ? cubeSize / fabsf(rayDirection.z) : 0.0f;

		// ���C�̎n�_���l���؂̍ŏ��_���܂ރm�[�h���琔���ĉ��ڂ̃m�[�h�ɋ��邩�Z�o�i�O�ڃX�^�[�g�j
		uint32_t nowX = static_cast<uint32_t>((rayStart.x - routeMinPoint.x) / cubeSize);
		uint32_t nowZ = static_cast<uint32_t>((rayStart.z - routeMinPoint.z) / cubeSize);

		// ���C�̏I�_���l���؂̍ŏ��_���܂ރm�[�h���琔���ĉ��ڂ̃m�[�h�ɋ��邩�Z�o
		uint32_t goalX = static_cast<uint32_t>((rayEnd.x - routeMinPoint.x) / cubeSize);
		uint32_t goalZ = static_cast<uint32_t>((rayEnd.z - routeMinPoint.z) / cubeSize);

		// ���C�̎n�_�̃m�[�h�̍ŏ��E�ő���W�̎Z�o
		float minX = routeMinPoint.x + nowX * cubeSize, maxX = minX + cubeSize;
		float minZ = routeMinPoint.z + nowZ * cubeSize, maxZ = minZ + cubeSize;

		// ���C�̎n�_�ɂ����āA���C���i�ލہA�u���̃m�[�h�ɂԂ���܂ł̋���/|���C�̐���|�v���Z�o
		float distX = directionX * (maxX - rayStart.x) + (1 - directionX) * cubeSize / 2;
		float distZ = directionZ * (maxZ - rayStart.z) + (1 - directionZ) * cubeSize / 2;

		float tx = directionX != 0 ? distX / fabsf(rayDirection.x) : FLT_MAX;
		float tz = directionZ != 0 ? distZ / fabsf(rayDirection.z) : FLT_MAX;


		// ���[�v�����Ŏl���؂̓���K�w���̃��Cvs�O�p�`�̌���������s��
		while (1)
		{
			hit = false;

			// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
			uint32_t linerIndex = mortonCode + levelStart;

			// �K�w���Ɏ��܂��Ă��邩�m�F
			if (linerIndex <= GetLevelStart(level + 1) - 1)
			{
				QuadtreeNode targetNode = m_quadtreeNodes[linerIndex];

				// �e�m�[�h�����O�p�`�S�Ăƃ��C�̓����蔻����s��
				for (int i = 0; i < targetNode.GetTriangles().size(); i++)
				{
					HitResultVector tmpResult;
					DirectX::XMVECTOR triangleVerts[3] = {
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
						DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
					};
					if (Collision::IntersectRayVsTriangle(DirectX::XMLoadFloat3(&rayStart), DirectX::XMLoadFloat3(&rayDirection), triangleVerts, tmpResult))
					{
						if (result.distance > tmpResult.distance)
						{
							result.distance = tmpResult.distance;
							result.normal = tmpResult.normal;
							result.position = tmpResult.position;
							result.triangleVerts[0] = tmpResult.triangleVerts[0];
							result.triangleVerts[1] = tmpResult.triangleVerts[1];
							result.triangleVerts[2] = tmpResult.triangleVerts[2];
							result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
							ret = true;
						}
						hit = true;
					}
				}
			}
			else
			{
				// �K�w�O�Ȃ�I��
				break;
			}

			// �P�x�ł��q�b�g���Ă�����A���̊K�w�̃`�F�b�N�͏I���B�i���C�̊J�n�_�ɋ߂��m�[�h���Ƀ`�F�b�N���Ă���̂Łj
			if (hit) break;

			// DDA��p���āA���Ƀ`�F�b�N����ׂ̋����Z�o���A���̃m�[�h���������[�g���R�[�h���㏑������
			// x����
			if (tx <= tz)
			{
				// �I�[�̋�Ԃ܂ŗ��Ă����猻�݂̊K�w�̏������I��������
				if (nowX == goalX)	break;

				// ���Ɍ������Ă���ŒZ�������������T�������Ԃ܂ł̋����������Ȃ��Ă���΁A
				// ����ȏ�i�ޕK�v�͂Ȃ����ߌ��݂̊K�w�̏������I��������
				if (result.distance < rayDist * tx)	break;

				// �ړ�����̂�tx��nowX���X�V����
				tx += dx;
				nowX += directionX;

				// �ׂ̋�Ԃ̃��[�g���R�[�h�i����`�j���Z�o���㏑������
				mortonCode = GetNextMortonCode(mortonCode, 0, directionX == -1);
			}
			// z����
			else
			{
				// �I�[�̋�Ԃ܂ŗ��Ă����珈�����I��������
				if (nowZ == goalZ)	break;

				// ���Ɍ������Ă���ŒZ�������������T�������Ԃ܂ł̋����������Ȃ��Ă���΁A
				// ����ȏ�i�ޕK�v�͂Ȃ����ߌ��݂̊K�w�̏������I��������
				if (result.distance < rayDist * tz)	break;

				// �ړ�����̂�tz��nowZ���X�V����
				tz += dz;
				nowZ += directionZ;

				// �ׂ̋�Ԃ̃��[�g���R�[�h�i����`�j���Z�o���㏑������
				mortonCode = GetNextMortonCode(mortonCode, 1, directionZ == -1);
			}
		}
	}

	return ret;
}

/**************************************************************************//**
	@brief		�S�Ẵm�[�h�ŎO�p�`�ƃX�t�B�A�L���X�g�̌���������s���i�ŏ��Ɍ�������O�p�`�݂̂�Ԃ������j
	@param[in]	XMFLOAT3 rayStart : �X�t�B�A�L���X�g�̎n�_
				XMFLOAT3 rayEnd : �X�t�B�A�L���X�g�̏I�_
				float radius : �X�t�B�A�̔��a
				HitResultVector result : �q�b�g���ʋL�^�p�ϐ�
				vector<CrossedNode>* nodeList : ���������m�[�h���X�g
	@return		bool : ��������Ƃ�true
*//***************************************************************************/
bool QuadtreeNodeManager::IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result)
{
	std::vector<QuadtreeNodeManager::CrossedNode>	mortonList;	// ���������m�[�h���L�^����
	return IntersectSphereCastVsTriangle(rayStart, rayEnd, radius, result, &mortonList);
}
bool QuadtreeNodeManager::IntersectSphereCastVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, float radius, HitResultVector& result, std::vector<CrossedNode>* nodeList)
{
	// �l���؂̍ŉ��w�̕Ӓ������X�t�B�A�L���X�g�̔��a���傫���ꍇ�A����̃A���S���Y���ł͔���o���Ȃ����߁A�ŉ��w�����̊K�w�Ɉړ�������
	float minSize = m_quadtreeNodes.back().GetHalfSize() * 2.0f;
	uint32_t targetDepth = m_depth;
	while (minSize < radius)
	{
		minSize *= 2.0f;
		targetDepth--;
	}

	// ����p�ϐ�
	bool ret = false;			// �S�̂̌������ʁB��x�ł��O�p�`�Ƃ̌������o���true
	bool hitNowLevel = false;	// ���݂̊K�w�ł̌������ʁB��x�ł��O�p�`�Ƃ̌������o���true
	bool hitRayCast = false;	// �P��̃��C�L���X�g�ł̌������ʁB��x�ł��O�p�`�Ƃ̌������o���true

	uint32_t level = 0;

	// ���C�̒����Ńq�b�g���̋��������������A���K�������Ă���
	DirectX::XMFLOAT3 rayDirection =
	{
		rayEnd.x - rayStart.x,
		0.0f,
		rayEnd.z - rayStart.z,
	};
	float rayDist = sqrtf(rayDirection.x * rayDirection.x + rayDirection.z * rayDirection.z);
	rayDirection.x /= rayDist;
	rayDirection.z /= rayDist;
	result.distance = rayDist;

	// �l���؋�Ԃ̍ŏ��l�ix,z���W�j
	DirectX::XMFLOAT3 routeMinPoint = m_quadtreeNodes.front().GetCenter();
	routeMinPoint.x -= m_quadtreeNodes.front().GetHalfSize();
	routeMinPoint.z -= m_quadtreeNodes.front().GetHalfSize();

	// ���C�̌������P�E�O�E�|�P�ŋL�^
	int directionX = rayDirection.x > 0.0f ? 1 : (rayDirection.x < 0.0f ? -1 : 0);
	int directionZ = rayDirection.z > 0.0f ? 1 : (rayDirection.z < 0.0f ? -1 : 0);

	// �m�[�h���w�肵�A���̃m�[�h���̑S�Ă̎O�p�`�ƃX�t�B�A�L���X�g���s�������_��
	auto IntersectShpereCastVsTriangleInTargetNode = [&](int linerIndex)
	{
		QuadtreeNode targetNode = m_quadtreeNodes[linerIndex];

		// �e�m�[�h�����O�p�`�S�Ăƃ��C�̓����蔻����s��
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			HitResult tmpResult;
			DirectX::XMVECTOR triangleVerts[3] = {
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectSphereCastVsTriangle(DirectX::XMLoadFloat3(&rayStart), DirectX::XMLoadFloat3(&rayDirection), rayDist, radius, triangleVerts, &tmpResult, true))
			{
				if (result.distance > tmpResult.distance)
				{
					result.distance = tmpResult.distance;
					result.normal = DirectX::XMLoadFloat3(&tmpResult.normal);
					result.position = DirectX::XMLoadFloat3(&tmpResult.position);
					result.triangleVerts[0] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[0]);
					result.triangleVerts[1] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[1]);
					result.triangleVerts[2] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[2]);
					result.materialIndex = targetNode.GetTriangles().at(i).materialIndex;
					ret = true;
				}
				hitRayCast = true;
				hitNowLevel = true;
			}
		}
	};
	// ���ɓo�^�ς݂̃C���f�b�N�X���m�F���Ă���A��L�����_�����Ăяo�������_���B���ꂾ����OK
	auto resisterAndIntersectShpereCastVsTriangleInTargetNode = [&](uint32_t linerIndex)
	{
		if (linerIndex <= GetLevelStart(level + 1) - 1)
		{
			if (!std::any_of(nodeList->begin(), nodeList->end(),
				[linerIndex](const CrossedNode& data) { return data.m_linerIndex == linerIndex; }))
			{
				CrossedNode mortonData;
				mortonData.m_linerIndex = linerIndex;
				mortonData.m_level = level;
				nodeList->emplace_back(mortonData);
				IntersectShpereCastVsTriangleInTargetNode(linerIndex);
			}
		}
		else
		{
			return false;
		}

		return true;
	};

	// �K�w�ilevel�j���̃��[�v
	for (level = 0; level <= targetDepth; level++)
	{
		hitNowLevel = false;

		uint32_t levelStart = GetLevelStart(level);

		// ���C�ŒH��m�[�h���Z�o���邽�߂̊e�ϐ��̏���
		float cubeSize = m_quadtreeNodes[levelStart].GetHalfSize() * 2;

		// ���C�̌X�������m�[�h��i�߂����̎��̃m�[�h�܂ł̋����̑����ʂ̒萔�u�����̂�xz���̕ӂ̒���/|���C�̐���|�v���Z�o
		float dx = directionX != 0 ? cubeSize / fabsf(rayDirection.x * rayDist) : 0.0f;
		float dz = directionZ != 0 ? cubeSize / fabsf(rayDirection.z * rayDist) : 0.0f;

		// ���C�̎n�_�̌�������
		uint32_t mortonCode = GetMortonCode(rayStart, m_quadtreeNodes.front(), m_quadtreeNodes[levelStart].GetHalfSize());
		if (!resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + levelStart))
		{
			continue;
		}

		// ���C�̍��W�␳�l
		DirectX::XMFLOAT3 fixAddVec = {};
		// ���S�A���A�E����̃��C�Ǝl���؂̌�������
		for (int i = 0; i < 3; i++)
		{
			// ���C�̍��W�␳�l���Z�o
			if (i == 0)	// ���S
			{
				fixAddVec = { rayDirection.x * radius, 0.0f, rayDirection.z * radius };
			}
			else if (i == 1)	// ��
			{
				fixAddVec = { rayDirection.z * radius, 0.0f, -rayDirection.x * radius };
			}
			else if (i == 2)	// �E
			{
				fixAddVec = { -rayDirection.z * radius, 0.0f, rayDirection.x * radius };
			}

			// ���W�␳��̃��C�̎n�_�̃��[�g���R�[�h
			mortonCode = GetMortonCode(DirectX::XMFLOAT3{ rayStart.x + fixAddVec.x, 0.0f, rayStart.z + fixAddVec.z }, m_quadtreeNodes.front(), m_quadtreeNodes[levelStart].GetHalfSize());

			// ���C�̎n�_���l���؂̍ŏ��_���܂ރm�[�h���琔���ĉ��ڂ̃m�[�h�ɋ��邩�Z�o�i�O�ڃX�^�[�g�j
			uint32_t nowX = static_cast<uint32_t>((rayStart.x + fixAddVec.x - routeMinPoint.x) / cubeSize);
			uint32_t nowZ = static_cast<uint32_t>((rayStart.z + fixAddVec.z - routeMinPoint.z) / cubeSize);
			// ���C�̏I�_���l���؂̍ŏ��_���܂ރm�[�h���琔���ĉ��ڂ̃m�[�h�ɋ��邩�Z�o
			uint32_t goalX = static_cast<uint32_t>((rayEnd.x + fixAddVec.x - routeMinPoint.x) / cubeSize);
			uint32_t goalZ = static_cast<uint32_t>((rayEnd.z + fixAddVec.z - routeMinPoint.z) / cubeSize);
			// ���C�̎n�_�̃m�[�h�̍ŏ��E�ő���W�̎Z�o
			float minX = routeMinPoint.x + nowX * cubeSize, maxX = minX + cubeSize;
			float minZ = routeMinPoint.z + nowZ * cubeSize, maxZ = minZ + cubeSize;
			// ���C���i�ލہA���̃m�[�h�ɂԂ���܂ł̋����i���C�̔䗦�j���Z�o
			float distX = directionX * (maxX - (rayStart.x + fixAddVec.x)) + (1 - directionX) * cubeSize / 2;
			float distZ = directionZ * (maxZ - (rayStart.z + fixAddVec.z)) + (1 - directionZ) * cubeSize / 2;

			float tx = directionX != 0 ? distX / fabsf(rayDirection.x * rayDist) : FLT_MAX;
			float tz = directionZ != 0 ? distZ / fabsf(rayDirection.z * rayDist) : FLT_MAX;

			// ���[�v�����Ŏl���؂̓���K�w���̃X�t�B�A�L���X�gvs�O�p�`�̌���������s��
			while (1)
			{
				hitRayCast = false;

				// �o�^�ƌ������菈��
				if (!resisterAndIntersectShpereCastVsTriangleInTargetNode(mortonCode + levelStart))
				{
					break;
				}

				// �P�x�ł��q�b�g���Ă�����A���̊K�w�̃`�F�b�N�͏I���B�i���C�̊J�n�_�ɋ߂��m�[�h���Ƀ`�F�b�N���Ă���̂Łj
				if (hitRayCast) break;

				// DDA��p���āA���Ƀ`�F�b�N����ׂ̋����Z�o���A���̃m�[�h���������[�g���R�[�h���㏑������
				// x����
				if (tx <= tz)
				{
					// �I�[�̋�Ԃ܂ŗ��Ă����猻�݂̊K�w�̏������I��������
					if (nowX == goalX)	break;

					// ���Ɍ������Ă���ŒZ�������������T�������Ԃ܂ł̋����������Ȃ��Ă���΁A
					// ����ȏ�i�ޕK�v�͂Ȃ����ߌ��݂̊K�w�̏������I��������
					if (result.distance < rayDist * tx)	break;

					// �ړ�����̂�tx��nowX���X�V����
					tx += dx;
					nowX += directionX;

					// �ׂ̋�Ԃ̃��[�g���R�[�h�i����`�j���Z�o���㏑������
					mortonCode = GetNextMortonCode(mortonCode, 0, directionX == -1);
				}
				// z����
				else
				{
					// �I�[�̋�Ԃ܂ŗ��Ă����珈�����I��������
					if (nowZ == goalZ)	break;

					// ���Ɍ������Ă���ŒZ�������������T�������Ԃ܂ł̋����������Ȃ��Ă���΁A
					// ����ȏ�i�ޕK�v�͂Ȃ����ߌ��݂̊K�w�̏������I��������
					if (result.distance < rayDist * tz)	break;

					// �ړ�����̂�tz��nowZ���X�V����
					tz += dz;
					nowZ += directionZ;

					// �ׂ̋�Ԃ̃��[�g���R�[�h�i����`�j���Z�o���㏑������
					mortonCode = GetNextMortonCode(mortonCode, 1, directionZ == -1);
				}
			}
		}

		// �����܂łŌ������Ȃ���΁A�X�t�B�A�L���X�g�̏I�[�̋��Ǝ��͂̋�Ԃ̔�����s��
		if (!hitNowLevel)
		{
			uint32_t mortonCode = GetMortonCode(rayEnd, m_quadtreeNodes.front(), m_quadtreeNodes[levelStart].GetHalfSize());
			uint32_t tmpMortonCode = mortonCode;
			// ���C�̏I�_���l���؂̍ŏ��_���܂ރm�[�h���琔���ĉ��ڂ̃m�[�h�ɋ��邩�Z�o
			uint32_t endX = static_cast<uint32_t>((rayEnd.x - routeMinPoint.x) / cubeSize);
			uint32_t endZ = static_cast<uint32_t>((rayEnd.z - routeMinPoint.z) / cubeSize);
			// ���C�̏I�_�̃m�[�h�̍ŏ��E�ő���W�̎Z�o
			float minX = routeMinPoint.x + endX * cubeSize, maxX = minX + cubeSize;
			float minZ = routeMinPoint.z + endZ * cubeSize, maxZ = minZ + cubeSize;

			// 8�����̗אڂ����Ԃ��`�F�b�N����
			float distLeft = rayEnd.x - minX;
			float distRight = maxX - rayEnd.x;
			float distDown = rayEnd.z - minZ;
			float distUp = maxZ - rayEnd.z;
			uint32_t divisions = 1 << level;

			if (distLeft < radius && endX > 0)
			{
				tmpMortonCode = GetNextMortonCode(mortonCode, 0, true);
				resisterAndIntersectShpereCastVsTriangleInTargetNode(tmpMortonCode + levelStart);

				if (distLeft * distLeft + distDown * distDown < radius * radius && endZ > 0)
				{
					resisterAndIntersectShpereCastVsTriangleInTargetNode(GetNextMortonCode(tmpMortonCode, 1, true) + levelStart);
				}

				if (distLeft * distLeft + distUp * distUp < radius * radius && endZ < divisions)
				{
					resisterAndIntersectShpereCastVsTriangleInTargetNode(GetNextMortonCode(tmpMortonCode, 1, false) + levelStart);
				}
			}

			if (distRight < radius && endX < divisions)
			{
				tmpMortonCode = GetNextMortonCode(mortonCode, 0, false);
				resisterAndIntersectShpereCastVsTriangleInTargetNode(tmpMortonCode + levelStart);

				if (distRight * distRight + distDown * distDown < radius * radius && endZ > 0)
				{
					resisterAndIntersectShpereCastVsTriangleInTargetNode(GetNextMortonCode(tmpMortonCode, 1, true) + levelStart);
				}

				if (distRight * distRight + distUp * distUp < radius * radius && endZ < divisions)
				{
					resisterAndIntersectShpereCastVsTriangleInTargetNode(GetNextMortonCode(tmpMortonCode, 1, false) + levelStart);
				}
			}

			if (distDown < radius && endZ > 0)
			{
				tmpMortonCode = GetNextMortonCode(mortonCode, 1, true);
				resisterAndIntersectShpereCastVsTriangleInTargetNode(tmpMortonCode + levelStart);
			}

			if (distUp < radius && endZ < divisions)
			{
				tmpMortonCode = GetNextMortonCode(mortonCode, 1, false);
				resisterAndIntersectShpereCastVsTriangleInTargetNode(tmpMortonCode + levelStart);
			}
		}
	}

	return ret;
}

/**************************************************************************//**
	@brief		�w�肵���m�[�h�ȉ��S�Ẵm�[�h�ŃI�u�W�F�N�g�ƃI�u�W�F�N�g�̉����߂����������s
	@param[in]	uint32_t target : �w��m�[�h
				bool singleNode : �P���Ԏw��
				vector<Sphere*>* upperListSphere : ��ʃm�[�h�̋���
				vector<Capsule*>* upperListCapsule : ��ʃm�[�h�̃J�v�Z��
				vector<AABB*>* upperListAABB : ��ʃm�[�h��AABB
				vector<Triangle>* upperListTriangle : ��ʃm�[�h�̎O�p�`
	@return		bool : ��������Ƃ�true
*//***************************************************************************/
// �S�ẴI�u�W�F�N�g�̉����߂����������s
void QuadtreeNodeManager::CollisionAllObjects(
	uint32_t target,
	bool singleNode,
	std::vector<Sphere*>* upperListSphere,
	std::vector<Capsule*>* upperListCapsule,
	std::vector<AABB*>* upperListAABB,
	std::vector<Triangle>* upperListTriangle)
{
	CollisionSphereVsSphere(target, singleNode, upperListSphere);
	CollisionCapsuleVsCapsule(target, singleNode, upperListCapsule);
	CollisionAABBVsAABB(target, singleNode, upperListAABB);
	CollisionSphereVsCapsule(target, singleNode, upperListSphere, upperListCapsule);
	CollisionSphereVsAABB(target, singleNode, upperListSphere, upperListAABB);
	CollisionCapsuleVsAABB(target, singleNode, upperListCapsule, upperListAABB);
	CollisionSphereVsTriangle(target, singleNode, upperListSphere, upperListTriangle);
	CollisionCapsuleVsTriangle(target, singleNode, upperListCapsule, upperListTriangle);
	CollisionAABBVsTriangle(target, singleNode, upperListAABB, upperListTriangle);
}
// ���� VS ����
void QuadtreeNodeManager::CollisionSphereVsSphere(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// �����ԓ��̏Փˏ���
		for (int j = i + 1; j < targetNode.GetSpheres().size(); j++)
		{
			if (Collision::IntersectSphereVsSphere(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(j)->position), 
				targetNode.GetSpheres().at(j)->radius, 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetSpheres().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetSpheres().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetSpheres().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�ԂƂ̏Փˏ���
		for (int j = 0; upperListSphere && j < upperListSphere->size(); j++)
		{
			if (Collision::IntersectSphereVsSphere(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&upperListSphere->at(j)->position), 
				upperListSphere->at(j)->radius, &result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListSphere->at(j)->position.x -= addVecF.x * 0.5f;
				upperListSphere->at(j)->position.y -= addVecF.y * 0.5f;
				upperListSphere->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂�UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionSphereVsSphere(nextTarget, false, upperListSphere);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
		}
	}
}
// �J�v�Z�� VS �J�v�Z��
void QuadtreeNodeManager::CollisionCapsuleVsCapsule(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// �����ԓ��̏Փˏ���
		for (int j = i + 1; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectCapsuleVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction),
				targetNode.GetCapsules().at(j)->length,
				targetNode.GetCapsules().at(j)->radius,
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�ԂƂ̏Փˏ���
		for (int j = 0; upperListCapsule && j < upperListCapsule->size(); j++)
		{
			if (Collision::IntersectCapsuleVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->direction),
				upperListCapsule->at(j)->length,
				upperListCapsule->at(j)->radius,
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				upperListCapsule->at(j)->position.x -= addVecF.x * 0.5f;
				upperListCapsule->at(j)->position.y -= addVecF.y * 0.5f;
				upperListCapsule->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂�UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionCapsuleVsCapsule(nextTarget, false, upperListCapsule);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
		}
	}
}
// AABB VS AABB
void QuadtreeNodeManager::CollisionAABBVsAABB(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	for (int i = 0; i < targetNode.GetAABBs().size(); i++)
	{
		// �����ԓ��̏Փˏ���
		for (int j = i + 1; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectAABBVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�ԂƂ̏Փˏ���
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectAABBVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii),
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂�UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionAABBVsAABB(nextTarget, false, upperListAABB);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}
// ���� VS �O�p�`
void QuadtreeNodeManager::CollisionSphereVsTriangle(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Triangle>* upperListTriangle)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// ���̑��̏���
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// �����ԓ��̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				triPos, 
				&result))
			{
				// ���̂̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z;
			}
		}
		// ��ʋ�Ԃ̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				triPos, 
				&result))
			{
				// ���̂̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z;
			}
		}
	}

	// �O�p�`���̏���
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// ��ʋ�Ԃ̋��̂Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectSphereVsTriangle(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position), 
				upperListSphere->at(i)->radius, 
				triPos, 
				&result))
			{
				// ���̂̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x;
				upperListSphere->at(i)->position.y += addVecF.y;
				upperListSphere->at(i)->position.z += addVecF.z;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂ƎO�p�`��UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionSphereVsTriangle(nextTarget, false, upperListSphere, upperListTriangle);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// �J�v�Z�� VS �O�p�`
void QuadtreeNodeManager::CollisionCapsuleVsTriangle(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<Triangle>* upperListTriangle)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// �J�v�Z�����̏���
	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// �����ԓ��̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction), 
				targetNode.GetCapsules().at(i)->radius, 
				targetNode.GetCapsules().at(i)->length, 
				triPos, 
				&result))
			{
				// �J�v�Z���̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z;
			}
		}
		// ��ʋ�Ԃ̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction), 
				targetNode.GetCapsules().at(i)->radius, 
				targetNode.GetCapsules().at(i)->length, 
				triPos, 
				&result))
			{
				// �J�v�Z���̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z;
			}
		}
	}

	// �O�p�`���̏���
	for (int i = 0; upperListCapsule && i < upperListCapsule->size(); i++)
	{
		// ��ʋ�Ԃ̃J�v�Z���Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectCapsuleVsTriangle(
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->position), 
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->direction), 
				upperListCapsule->at(i)->radius, 
				upperListCapsule->at(i)->length, 
				triPos, 
				&result))
			{
				// �J�v�Z���̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListCapsule->at(i)->position.x += addVecF.x;
				upperListCapsule->at(i)->position.y += addVecF.y;
				upperListCapsule->at(i)->position.z += addVecF.z;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă���J�v�Z���ƎO�p�`��UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionCapsuleVsTriangle(nextTarget, false, upperListCapsule, upperListTriangle);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// AABB VS �O�p�`
void QuadtreeNodeManager::CollisionAABBVsTriangle(uint32_t target, bool singleNode, std::vector<AABB*>* upperListAABB, std::vector<Triangle>* upperListTriangle)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// AABB���̏���
	for (int i = 0; i < targetNode.GetAABBs().size(); i++)
	{
		// �����ԓ��̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii), 
				triPos, 
				&result))
			{
				// AABB�̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z;
			}
		}
		// ��ʋ�Ԃ̎O�p�`�Ƃ̏Փˏ���
		for (int j = 0; upperListTriangle && j < upperListTriangle->size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[0]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[1]),
				DirectX::XMLoadFloat3(&upperListTriangle->at(j).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(i)->radii), 
				triPos, 
				&result))
			{
				// AABB�̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetAABBs().at(i)->position.x += addVecF.x;
				targetNode.GetAABBs().at(i)->position.y += addVecF.y;
				targetNode.GetAABBs().at(i)->position.z += addVecF.z;
			}
		}
	}

	// �O�p�`���̏���
	for (int i = 0; upperListAABB && i < upperListAABB->size(); i++)
	{
		// ��ʋ�Ԃ�AABB�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetTriangles().size(); j++)
		{
			DirectX::XMVECTOR triPos[3] =
			{
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[0]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[1]),
				DirectX::XMLoadFloat3(&targetNode.GetTriangles().at(i).position[2])
			};
			if (Collision::IntersectAABBVsTriangle(
				DirectX::XMLoadFloat3(&upperListAABB->at(i)->position), 
				DirectX::XMLoadFloat3(&upperListAABB->at(i)->radii), 
				triPos, 
				&result))
			{
				// AABB�̂݉����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListAABB->at(i)->position.x += addVecF.x;
				upperListAABB->at(i)->position.y += addVecF.y;
				upperListAABB->at(i)->position.z += addVecF.z;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă���AABB�ƎO�p�`��UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetTriangles().size(); i++)
		{
			upperListTriangle->emplace_back(targetNode.GetTriangles().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionAABBVsTriangle(nextTarget, false, upperListAABB, upperListTriangle);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
			for (int i = 0; i < targetNode.GetTriangles().size(); i++)
			{
				upperListTriangle->pop_back();
			}
		}
	}
}
// ���� VS �J�v�Z��
void QuadtreeNodeManager::CollisionSphereVsCapsule(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<Capsule*>* upperListCapsule)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// ���̑��̏���
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// �����ԓ��̃J�v�Z���Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction), 
				targetNode.GetCapsules().at(j)->length, 
				targetNode.GetCapsules().at(j)->radius, 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�Ԃ̃J�v�Z���Ƃ̏Փˏ���
		for (int j = 0; upperListCapsule && j < upperListCapsule->size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->position), 
				DirectX::XMLoadFloat3(&upperListCapsule->at(j)->direction), 
				upperListCapsule->at(j)->length, 
				upperListCapsule->at(j)->radius, 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListCapsule->at(j)->position.x -= addVecF.x * 0.5f;
				upperListCapsule->at(j)->position.y -= addVecF.y * 0.5f;
				upperListCapsule->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// �J�v�Z�����̏���
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// ��ʋ�Ԃ̋��̂Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetCapsules().size(); j++)
		{
			if (Collision::IntersectSphereVsCapsule(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position), 
				upperListSphere->at(i)->radius, 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(j)->direction), 
				targetNode.GetCapsules().at(j)->length, 
				targetNode.GetCapsules().at(j)->radius, 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x * 0.5f;
				upperListSphere->at(i)->position.y += addVecF.y * 0.5f;
				upperListSphere->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetCapsules().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetCapsules().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetCapsules().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂ƃJ�v�Z����UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionSphereVsCapsule(nextTarget, false, upperListSphere, upperListCapsule);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
		}
	}
}
// ���� VS AABB
void QuadtreeNodeManager::CollisionSphereVsAABB(uint32_t target, bool singleNode, std::vector<Sphere*>* upperListSphere, std::vector<AABB*>* upperListAABB)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// ���̑��̏���
	for (int i = 0; i < targetNode.GetSpheres().size(); i++)
	{
		// �����ԓ���AABB�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii), 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�Ԃ�AABB�Ƃ̏Փˏ���
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetSpheres().at(i)->position), 
				targetNode.GetSpheres().at(i)->radius, 
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position), 
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii), 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetSpheres().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetSpheres().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetSpheres().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// AABB���̏���
	for (int i = 0; upperListSphere && i < upperListSphere->size(); i++)
	{
		// ��ʋ�Ԃ̋��̂Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectSphereVsAABB(
				DirectX::XMLoadFloat3(&upperListSphere->at(i)->position), 
				upperListSphere->at(i)->radius, 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position), 
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii), 
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListSphere->at(i)->position.x += addVecF.x * 0.5f;
				upperListSphere->at(i)->position.y += addVecF.y * 0.5f;
				upperListSphere->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă��鋅�̂�AABB��UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetSpheres().size(); i++)
		{
			upperListSphere->emplace_back(targetNode.GetSpheres().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionSphereVsAABB(nextTarget, false, upperListSphere, upperListAABB);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetSpheres().size(); i++)
			{
				upperListSphere->pop_back();
			}
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}
// �J�v�Z�� VS AABB
void QuadtreeNodeManager::CollisionCapsuleVsAABB(uint32_t target, bool singleNode, std::vector<Capsule*>* upperListCapsule, std::vector<AABB*>* upperListAABB)
{
	QuadtreeNode targetNode = m_quadtreeNodes[target];
	IntersectionResult result;

	// AABB���̏���
	for (int i = 0; i < targetNode.GetCapsules().size(); i++)
	{
		// �����ԓ��̃J�v�Z���Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
		// ��ʋ�Ԃ̃J�v�Z���Ƃ̏Փˏ���
		for (int j = 0; upperListAABB && j < upperListAABB->size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->position),
				DirectX::XMLoadFloat3(&targetNode.GetCapsules().at(i)->direction),
				targetNode.GetCapsules().at(i)->length,
				targetNode.GetCapsules().at(i)->radius,
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->position),
				DirectX::XMLoadFloat3(&upperListAABB->at(j)->radii),
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				targetNode.GetCapsules().at(i)->position.x += addVecF.x * 0.5f;
				targetNode.GetCapsules().at(i)->position.y += addVecF.y * 0.5f;
				targetNode.GetCapsules().at(i)->position.z += addVecF.z * 0.5f;
				upperListAABB->at(j)->position.x -= addVecF.x * 0.5f;
				upperListAABB->at(j)->position.y -= addVecF.y * 0.5f;
				upperListAABB->at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// �J�v�Z�����̏���
	for (int i = 0; upperListCapsule && i < upperListCapsule->size(); i++)
	{
		// ��ʋ�Ԃ�AABB�Ƃ̏Փˏ���
		for (int j = 0; j < targetNode.GetAABBs().size(); j++)
		{
			if (Collision::IntersectCapsuleVsAABB(
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->position),
				DirectX::XMLoadFloat3(&upperListCapsule->at(i)->direction),
				upperListCapsule->at(i)->length,
				upperListCapsule->at(i)->radius,
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->position),
				DirectX::XMLoadFloat3(&targetNode.GetAABBs().at(j)->radii),
				&result))
			{
				// �����߂�����
				DirectX::XMVECTOR addVec = DirectX::XMVectorScale(result.normal, result.penetration);
				DirectX::XMFLOAT3 addVecF = {};
				DirectX::XMStoreFloat3(&addVecF, addVec);
				upperListCapsule->at(i)->position.x += addVecF.x * 0.5f;
				upperListCapsule->at(i)->position.y += addVecF.y * 0.5f;
				upperListCapsule->at(i)->position.z += addVecF.z * 0.5f;
				targetNode.GetAABBs().at(j)->position.x -= addVecF.x * 0.5f;
				targetNode.GetAABBs().at(j)->position.y -= addVecF.y * 0.5f;
				targetNode.GetAABBs().at(j)->position.z -= addVecF.z * 0.5f;
			}
		}
	}

	// ���ʋ�Ԃ����݂��Ȃ����return
	if (target >= GetLevelStart(m_depth))	return;

	// �P���Ԏw��łȂ���΁A���ʋ�Ԃֈړ����ċN�Ăяo��
	if (!singleNode)
	{
		// �������Ă���AABB�ƃJ�v�Z����UpperList�ɓo�^���Ă���A�ċN�����Ŏq��ԂɈړ�����
		bool addUpperList = false;	// �o�^�����������ǂ����̃t���O�B�폜�ɗ��p����

		for (int i = 0; i < targetNode.GetCapsules().size(); i++)
		{
			upperListCapsule->emplace_back(targetNode.GetCapsules().at(i));
			addUpperList = true;
		}
		for (int i = 0; i < targetNode.GetAABBs().size(); i++)
		{
			upperListAABB->emplace_back(targetNode.GetAABBs().at(i));
			addUpperList = true;
		}

		for (int i = 1; i <= 4; i++)
		{
			uint32_t nextTarget = target * 4 + i;
			CollisionCapsuleVsAABB(nextTarget, false, upperListCapsule, upperListAABB);
		}

		// ���ʋ�Ԃ�S�ď������I�������AUpperList����폜����
		if (addUpperList)
		{
			for (int i = 0; i < targetNode.GetCapsules().size(); i++)
			{
				upperListCapsule->pop_back();
			}
			for (int i = 0; i < targetNode.GetAABBs().size(); i++)
			{
				upperListAABB->pop_back();
			}
		}
	}
}