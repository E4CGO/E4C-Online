#include "MapTileManager.h"

// ���C�L���X�g
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, bool camera)
{
	XMFLOAT3 rayDirection = end - start;
	float rayDist = XMFLOAT3Length(rayDirection);
	rayDirection = XMFLOAT3Normalize(rayDirection);

	return RayCast(start, rayDirection, rayDist, hit, camera);
}
bool MapTileManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& direction, float dist, HitResult& hit, bool camera)
{
	// �ŏ��l�ő�l
	float minX, maxX;
	if (direction.x > 0.0f)
	{
		minX = start.x;
		maxX = start.x + direction.x * dist;
	}
	else
	{
		minX = start.x + direction.x * dist;
		maxX = start.x;
	}

	float minY, maxY;
	if (direction.y > 0.0f)
	{
		minY = start.y;
		maxY = start.y + direction.y * dist;
	}
	else
	{
		minY = start.y + direction.y * dist;
		maxY = start.y;
	}

	float minZ, maxZ;
	if (direction.z > 0.0f)
	{
		minZ = start.z;
		maxZ = start.z + direction.z * dist;
	}
	else
	{
		minZ = start.z + direction.z * dist;
		maxZ = start.z;
	}

	// ���C���ʂ��Ԃ̔z��ԍ��Z�o
	int Elem = tree.GetLinerIndex(minX, maxX, minY, maxY, minZ, maxZ);

	bool ret = false;
	SearchChildren(Elem, start, direction, dist, hit, ret);
	SearchParent(Elem, start, direction, dist, hit, ret);
	return ret;
}

// �������C�L���X�g
bool MapTileManager::VerticalRayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	HitResultVector result;
	bool ret = quadtree.IntersectVerticalRayVsTriangle(start, end, result);

	XMStoreFloat3(&hit.position, result.position);
	XMStoreFloat3(&hit.normal, result.normal);
	hit.distance = result.distance;
	hit.materialIndex = result.materialIndex;
	XMStoreFloat3(&hit.triangleVerts[0], result.triangleVerts[0]);
	XMStoreFloat3(&hit.triangleVerts[1], result.triangleVerts[1]);
	XMStoreFloat3(&hit.triangleVerts[2], result.triangleVerts[2]);

	return ret;
}

// �}�b�v�T�C�Y�v�Z
void MapTileManager::CalcMapArea(DirectX::XMFLOAT3& minPos, DirectX::XMFLOAT3& maxPos)
{
	minPos = { FLT_MAX, FLT_MAX , FLT_MAX };
	maxPos = { FLT_MIN, FLT_MIN , FLT_MIN };

	for (ModelObject*& item : items)
	{
		if (item->GetCollider() == nullptr) continue;

		const ModelResource* resource = item->GetModel()->GetResource();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			// ���b�V���m�[�h�擾
			const iModel::Node& node = item->GetModel()->GetNodes().at(mesh.nodeIndex);
			// ���[���h�s��
			XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);

			// ���_�f�[�^�擾
			const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;

			for (UINT i = 1; i < vertices.size(); i++)
			{
				const XMVECTOR V = XMVector3Transform(DirectX::XMLoadFloat3(&vertices.at(i).position), WorldTransform);
				XMFLOAT3 tmpPos;
				XMStoreFloat3(&tmpPos, V);

				if (minPos.x > tmpPos.x)
				{
					minPos.x = tmpPos.x;
				}
				else if (maxPos.x < tmpPos.x)
				{
					maxPos.x = tmpPos.x;
				}

				if (minPos.y > tmpPos.y)
				{
					minPos.y = tmpPos.y;
				}
				else if (maxPos.y < tmpPos.y)
				{
					maxPos.y = tmpPos.y;
				}

				if (minPos.z > tmpPos.z)
				{
					minPos.z = tmpPos.z;
				}
				else if (maxPos.z < tmpPos.z)
				{
					maxPos.z = tmpPos.z;
				}
			}
		}
	}
}

void MapTileManager::CreateSpatialIndex(uint32_t quadDepth, uint32_t octDepth, DirectX::XMFLOAT3* minPos, DirectX::XMFLOAT3* maxPos)
{
	XMFLOAT3 c_minPos, c_maxPos;

	if (!minPos || !maxPos)
	{
		CalcMapArea(c_minPos, c_maxPos);
	}
	else
	{
		c_minPos = { minPos->x, minPos->y, minPos->z };
		c_maxPos = { maxPos->x, maxPos->y, maxPos->z };
	}

	tree.Initialize(octDepth,
		c_minPos.x - 1.0f, c_maxPos.x + 1.0f,
		c_minPos.y - 20.0f, c_maxPos.y + 20.0f,
		c_minPos.z - 1.0f, c_maxPos.z + 1.0f);	// �G���A�������傫�߂ɍ쐬

	XMFLOAT3 center = (c_minPos + c_maxPos) * 0.5f;
	XMFLOAT3 size = c_maxPos - c_minPos;
	float quadHalfSize = max(size.x, size.z) * 0.5f;
	float octHalfSize = max(size.y * 0.5f, quadHalfSize);
	quadHalfSize += 1.0f;
	octHalfSize += 1.0f;

	quadtree.CreateQuadtree(center, quadHalfSize, quadDepth);
	octree.CreateOctree(center, octHalfSize, octDepth);

	InsertMapMesh();
}

// �}�b�v�̃��b�V����o�^
int MapTileManager::InsertMapMesh()
{
	int count = 0;
	
	for (ModelObject*& item : items)
	{
		int nowMeshNum = 0;

		// ���f���̃��b�V���̎O�p�`�����[���h���W�Ŏl���؋�Ԃɓo�^
		const ModelResource* resource = item->GetModel()->GetResource();
		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			// ���b�V���m�[�h�擾
			const iModel::Node& node = item->GetModel()->GetNodes().at(mesh.nodeIndex);

			DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
			const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const std::vector<UINT> indices = mesh.indices;

			for (UINT i = 0; i < indices.size(); i += 3)
			{
				// �O�p�`�̒��_�𒊏o
				const ModelResource::Vertex& a = vertices.at(indices.at(i));
				const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));

				const DirectX::XMVECTOR A = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&a.position), WorldTransform);
				const DirectX::XMVECTOR B = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&b.position), WorldTransform);
				const DirectX::XMVECTOR C = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&c.position), WorldTransform);

				Triangle triangle = {};
				DirectX::XMStoreFloat3(&triangle.position[0], A);
				DirectX::XMStoreFloat3(&triangle.position[1], B);
				DirectX::XMStoreFloat3(&triangle.position[2], C);
				//triangle.materialIndex = mesh.materialIndex;
				Triangle* p_triangle = new Triangle;
				DirectX::XMStoreFloat3(&p_triangle->position[0], A);
				DirectX::XMStoreFloat3(&p_triangle->position[1], B);
				DirectX::XMStoreFloat3(&p_triangle->position[2], C);

				// �O�p�`�����[���h���W�œo�^
				quadtree.InsertTriangleObject(triangle);
				octree.InsertTriangleObject(triangle);

				XMFLOAT3 minPos, maxPos;
				triangle.GetBoundPoints(&minPos, &maxPos);
				tree.Regist(minPos.x, maxPos.x, minPos.y, maxPos.y, minPos.z, maxPos.z, p_triangle);

				count++;
			}
			nowMeshNum++;
		}
	}

	return count;
}

bool MapTileManager::SearchChildren(	// �q��ԒT��
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	// ��ԊO�Ȃ�return
	if (Elem < 0) return hit;

	// �q��Ԃ����݂���Ȃ�q��Ԃ�T��
	if ((Elem + 1) << 3 < tree.m_iCellNum)
	{
		int childElem = (Elem << 3) + 1;

		for (int i = 0; i < 8; i++)
		{
			if (SearchChildren(childElem + i, start, direction, dist, result, hit))
				break;
		}
	}

	// ���̋�Ԃ����݂��Ȃ��Ȃ�return
	if (!tree.ppCellAry[Elem])	return hit;

	// ���̋�Ԃɓo�^����Ă���|���S���Ƃ̓����蔻��
	Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[Elem]->pLatest;
	while (oft)
	{
		XMFLOAT3 triangleVerts[3] = {
			oft->m_pObject->position[0],
			oft->m_pObject->position[1],
			oft->m_pObject->position[2]
		};
		if (Collision::IntersectRayVsTriangle(start, direction, dist, triangleVerts, result))
		{
			hit = true;
		}
		
		oft = oft->m_pNext;
	}

	return hit;
}

bool MapTileManager::SearchParent(	// �e��ԒT��
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& direction,
	float dist,
	HitResult& result,
	bool& hit)
{
	// ��ԊO�Ȃ�return
	if (Elem <= 0) return hit;

	int parentElem = (Elem - 1) >> 3;
	while (1)
	{
		// ���̋�Ԃ����݂��Ȃ��Ȃ�e��ԂɈړ�
		if (!tree.ppCellAry[parentElem])
		{
			// ���[�g��ԂȂ�break
			if (parentElem == 0)	break;
			parentElem = (parentElem - 1) >> 3;
			continue;
		}

		Liner8TreeManager<Triangle>::OFT<Triangle>* oft = tree.ppCellAry[parentElem]->pLatest;
		while (oft)
		{
			XMFLOAT3 triangleVerts[3] = {
				oft->m_pObject->position[0],
				oft->m_pObject->position[1],
				oft->m_pObject->position[2]
			};
			if (Collision::IntersectRayVsTriangle(start, direction, dist, triangleVerts, result))
			{
				hit = true;
			}

			oft = oft->m_pNext;
		}

		// ���[�g��ԂȂ�break
		if (parentElem == 0)	break;

		// �e��ԂɈړ�
		parentElem = (parentElem - 1) >> 3;
	}

	return hit;
}
