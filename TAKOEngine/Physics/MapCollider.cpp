#include "TAKOEngine/Physics/MapCollider.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

MapCollider::MapCollider(Model* model)
{
	this->model = model;

	type = COLLIDER_TYPE::MAP;

	// 8���؍쐬
	XMFLOAT3 minPos, maxPos;
	CalcMapArea(minPos, maxPos);
	tree.Initialize(7,
		minPos.x - 1.0f, maxPos.x + 1.0f,
		minPos.y - 50.0f, maxPos.y + 10.0f,
		minPos.z - 1.0f, maxPos.z + 1.0f);// �G���A�������傫�߂ɍ쐬
	RegisterPorigons();
}

MapCollider::~MapCollider()
{
	tree.Finalize();
}

bool MapCollider::CollisionVsShpere(
	Collider*& other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	bool hit = false;
	DirectX::XMFLOAT3 sphereCenterOri = other->GetPosition();
	DirectX::XMFLOAT3 sphereCenter = other->GetPosition() + direction;

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// ���̂����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR SphereCenter = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenter), InverseWorldTransform);
		DirectX::XMVECTOR SphereCenterOri = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenterOri), InverseWorldTransform);
		float sphereRadius = other->GetScale().x; // ���̔��a
		DirectX::XMVECTOR SphereRadius = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat(&sphereRadius), InverseWorldTransform);
		DirectX::XMStoreFloat(&sphereRadius, SphereRadius);

		// �O�p�`�i�ʁj�Ƃ̌�������
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;

		for (UINT i = 0; i < indices.size(); i += 3)
		{
			// �O�p�`�̒��_�𒊏o
			//const ModelResource::Vertex& a = vertices.at(indices.at(i));
			//const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			//const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			const ModelResource::Vertex& c = vertices.at(indices.at(i));
			// �O�p�`���_
			DirectX::XMVECTOR TriangleVertex[3] =
			{
				DirectX::XMLoadFloat3(&a.position),
				DirectX::XMLoadFloat3(&b.position),
				DirectX::XMLoadFloat3(&c.position),
			};
			// �ʖ@��
			DirectX::XMVECTOR Edge[3] =
			{
				DirectX::XMVectorSubtract(TriangleVertex[1], TriangleVertex[0]),
				DirectX::XMVectorSubtract(TriangleVertex[2], TriangleVertex[1]),
				DirectX::XMVectorSubtract(TriangleVertex[0], TriangleVertex[2]),
			};
			DirectX::XMVECTOR TriangleNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Edge[0], Edge[1]));

			DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(TriangleNormal, SphereCenter);
			DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(TriangleNormal, TriangleVertex[0]);
			DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(TriangleNormal, SphereCenterOri);
			DirectX::XMVECTOR Distance = DirectX::XMVectorSubtract(Dot1, Dot2);
			DirectX::XMVECTOR DistanceOri = DirectX::XMVectorSubtract(Dot3, Dot2);
			float distance = DirectX::XMVectorGetX(Distance);
			float distanceOri = DirectX::XMVectorGetX(DistanceOri);

			// ���̒��S���ʂɐڂ��Ă��邩�A���̕����ɂ���ꍇ�͓�����Ȃ�
			if (distance < 0 && distanceOri < 0) continue;

			// �����O�p�`�����ɑ��݂��邩
			if (distance > sphereRadius) continue;

			bool outside = false;
			DirectX::XMVECTOR Vec[3];
			for (int i = 0; i < 3; ++i)
			{
				Vec[i] = DirectX::XMVectorSubtract(SphereCenter, TriangleVertex[i]);
				DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(Edge[i], Vec[i]);
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(TriangleNormal, Cross);
				if (DirectX::XMVectorGetX(Dot) < 0.0f)
				{
					outside = true;
				}
			}
			// �O�p�`�̓����Ȃ̂Ō�������
			if (!outside)
			{
				materialIndex = mesh.materialIndex;

				float depth = sphereRadius - distance;
				DirectX::XMVECTOR Reflection = DirectX::XMVectorScale(TriangleNormal, depth);
				SphereCenter = DirectX::XMVectorAdd(SphereCenter, Reflection);

				continue;
			}
			// �O��
			// �G�b�W�Ƃ̔���
			const float radiusSq = sphereRadius * sphereRadius;
			for (int i = 0; i < 3; ++i)
			{
				// �ӂ̎ˉe�l�����߂�
				float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec[i], Edge[i]));
				if (t > 0.0f)
				{
					// �ӂ̎n�_����I�_�܂ł̃x�N�g���Ǝx�X���狅�܂ł̃x�N�g��������̏ꍇ�A
					// ���ϒl���ӂ̒�����2��ɂȂ鐫���𗘗p���ĕӂ��狅�܂ł̍ŒZ�x�N�g�������߂�
					float edgeLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Edge[i]));
					if (t > edgeLengthSq)
					{
						Vec[i] = DirectX::XMVectorSubtract(Vec[i], Edge[i]);
					}
					else
					{
						t /= edgeLengthSq;
						Vec[i] = DirectX::XMVectorSubtract(Vec[i], DirectX::XMVectorScale(Edge[i], t));
					}
				}
				// �ӂ��狅�܂ł̍ŒZ�x�N�g���̋��������a�ȉ��Ȃ�߂荞��ł���
				float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec[i]));
				if (lengthSq < radiusSq)
				{
					materialIndex = mesh.materialIndex;
					// �߂荞�ݗʎZ�o
					float depth = sphereRadius - sqrtf(lengthSq);
					// �߂荞�ݕ������o������
					DirectX::XMVECTOR Reflection = DirectX::XMVector3Normalize(Vec[i]);
					Reflection = DirectX::XMVectorScale(Reflection, depth);
					SphereCenter = DirectX::XMVectorAdd(SphereCenter, Reflection);

					break;
				}
			}
		}
		if (materialIndex >= 0)
		{
			hit = true;
			result.materialIndex = materialIndex;

			// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(SphereCenter, WorldTransform);

			DirectX::XMStoreFloat3(&sphereCenter, WorldPosition);
		}
	}

	if (hit)
	{
		result.position = sphereCenter;
		result.normal = sphereCenter - other->GetPosition();
		result.distance = XMFLOAT3Length(result.normal);
		if (result.distance > 0.0f)
		{
			result.normal /= result.distance;
		}
	}

	return hit;
}

bool MapCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	// �ŏ��l�ő�l
	float minX, maxX;
	if (start.x < end.x)
	{
		minX = start.x;
		maxX = end.x;
	}
	else
	{
		minX = end.x;
		maxX = start.x;
	}
	
	float minY, maxY;
	if (start.y < end.y)
	{
		minY = start.y;
		maxY = end.y;
	}
	else
	{
		minY = end.y;
		maxY = start.y;
	}

	float minZ, maxZ;
	if (start.z < end.z)
	{
		minZ = start.z;
		maxZ = end.z;
	}
	else
	{
		minZ = end.z;
		maxZ = start.z;
	}

	// ���C���ʂ��Ԃ̔z��ԍ��Z�o
	int Elem = tree.GetLinerIndex(minX, maxX, minY, maxY, minZ, maxZ);

	bool hit = false;
	SearchChildren(Elem, start, end, result, hit);
	SearchParent(Elem, start, end, result, hit);
	return hit;
}

void MapCollider::CalcMapArea(
	DirectX::XMFLOAT3& minPos,
	DirectX::XMFLOAT3& maxPos
)
{
	minPos = { FLT_MAX, FLT_MAX , FLT_MAX };
	maxPos = { FLT_MIN, FLT_MIN , FLT_MIN };

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ���_�f�[�^�擾
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;

		XMFLOAT3 tmp_minPos = vertices.at(0).position;
		XMFLOAT3 tmp_maxPos = vertices.at(0).position;

		for (UINT i = 1; i < vertices.size(); i++)
		{
			if (vertices.at(i).position.x < tmp_minPos.x)
			{
				tmp_minPos.x = vertices.at(i).position.x;
			}
			else if (vertices.at(i).position.x > tmp_maxPos.x)
			{
				tmp_maxPos.x = vertices.at(i).position.x;
			}

			if (vertices.at(i).position.y < tmp_minPos.y)
			{
				tmp_minPos.y = vertices.at(i).position.y;
			}
			else if (vertices.at(i).position.y > tmp_maxPos.y)
			{
				tmp_maxPos.y = vertices.at(i).position.y;
			}

			if (vertices.at(i).position.z < tmp_minPos.z)
			{
				tmp_minPos.z = vertices.at(i).position.z;
			}
			else if (vertices.at(i).position.z > tmp_maxPos.z)
			{
				tmp_maxPos.z = vertices.at(i).position.z;
			}
		}

		// ���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		// ���[���h�s��
		XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);

		// ���_�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
		XMVECTOR MinP = XMLoadFloat3(&tmp_minPos);
		XMVECTOR MaxP = XMLoadFloat3(&tmp_maxPos);
		MinP = XMVector3TransformCoord(MinP, WorldTransform);
		MaxP = XMVector3TransformCoord(MaxP, WorldTransform);
		XMStoreFloat3(&tmp_minPos, MinP);
		XMStoreFloat3(&tmp_maxPos, MaxP);
		// �����Œl�̑召���t�ɂȂ��Ă�\��������̂ł�����x��r
		if (tmp_minPos.x > tmp_maxPos.x)
		{
			float tmp = tmp_maxPos.x;
			tmp_maxPos.x = tmp_minPos.x;
			tmp_minPos.x = tmp;
		}
		if (tmp_minPos.y > tmp_maxPos.y)
		{
			float tmp = tmp_maxPos.y;
			tmp_maxPos.y = tmp_minPos.y;
			tmp_minPos.y = tmp;
		}
		if (tmp_minPos.z > tmp_maxPos.z)
		{
			float tmp = tmp_maxPos.z;
			tmp_maxPos.z = tmp_minPos.z;
			tmp_minPos.z = tmp;
		}

		// ����܂łɌ��������̂Ɣ�r
		if (minPos.x > tmp_minPos.x)
		{
			minPos.x = tmp_minPos.x;
		}
		if (maxPos.x < tmp_maxPos.x)
		{
			maxPos.x = tmp_maxPos.x;
		}

		if (minPos.y > tmp_minPos.y)
		{
			minPos.y = tmp_minPos.y;
		}
		if (maxPos.y < tmp_maxPos.y)
		{
			maxPos.y = tmp_maxPos.y;
		}

		if (minPos.z > tmp_minPos.z)
		{
			minPos.z = tmp_minPos.z;
		}
		if (maxPos.z < tmp_maxPos.z)
		{
			maxPos.z = tmp_maxPos.z;
		}
	}
}

void MapCollider::RegisterPorigons()
{
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		
		// ���[���h�s��擾
		XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);

		// ���_�f�[�^�擾
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		for (UINT i = 0; i < indices.size(); i += 3)//102837
		{
			// �O�p�`�̒��_�𒊏o
			//const ModelResource::Vertex& a = vertices.at(indices.at(i));
			//const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			//const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			const ModelResource::Vertex& c = vertices.at(indices.at(i));

			XMVECTOR A = XMLoadFloat3(&a.position);
			XMVECTOR B = XMLoadFloat3(&b.position);
			XMVECTOR C = XMLoadFloat3(&c.position);

			// �O�p�`�̎O�Ӄx�N�g�����Z�o
			XMVECTOR AB = XMVectorSubtract(B, A);
			XMVECTOR BC = XMVectorSubtract(C, B);
			XMVECTOR CA = XMVectorSubtract(A, C);

			// �O�p�`�̖@���x�N�g�����Z�o
			XMVECTOR N = XMVector3Cross(AB, BC);

			// ���_�Ɩ@�������[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
			A = XMVector3TransformCoord(A, WorldTransform);
			B = XMVector3TransformCoord(B, WorldTransform);
			C = XMVector3TransformCoord(C, WorldTransform);
			N = XMVector3TransformNormal(N, WorldTransform);
			N = XMVector3Normalize(N);

			// Porigon�\���̂Ɋi�[
			Porigon* porigon = new Porigon;
			XMStoreFloat3(&porigon->position[0], A);

			//// ���_�̏��Ԃ�␳
			XMVECTOR WorldAB = XMVectorSubtract(B, A);
			XMVECTOR WorldAC = XMVectorSubtract(C, A);
			if (XMVectorGetX(XMVector3Dot(N, XMVector3Cross(WorldAB, WorldAC))) > 0)
			{
				XMStoreFloat3(&porigon->position[1], B);
				XMStoreFloat3(&porigon->position[2], C);
			}
			else
			{
				XMStoreFloat3(&porigon->position[1], C);
				XMStoreFloat3(&porigon->position[2], B);
			}
			
			XMStoreFloat3(&porigon->normal, N);
			porigon->materialIndex = mesh.materialIndex;


			/////////// 8���؂ɓo�^ ////////////
			// ���_�ʒu�̍ŏ��l�ő�l
			float minX, maxX;
			if (porigon->position[0].x < porigon->position[1].x)
			{
				if (porigon->position[0].x < porigon->position[2].x)
				{
					minX = porigon->position[0].x;
					maxX = porigon->position[1].x < porigon->position[2].x ?
						porigon->position[2].x : porigon->position[1].x;
				}
				else
				{
					minX = porigon->position[2].x;
					maxX = porigon->position[1].x;
				}
			}
			else
			{
				if (porigon->position[1].x < porigon->position[2].x)
				{
					minX = porigon->position[1].x;
					maxX = porigon->position[0].x < porigon->position[2].x ?
						porigon->position[2].x : porigon->position[0].x;
				}
				else
				{
					minX = porigon->position[2].x;
					maxX = porigon->position[0].x;
				}
			}

			float minY, maxY;
			if (porigon->position[0].y < porigon->position[1].y)
			{
				if (porigon->position[0].y < porigon->position[2].y)
				{
					minY = porigon->position[0].y;
					maxY = porigon->position[1].y < porigon->position[2].y ?
						porigon->position[2].y : porigon->position[1].y;
				}
				else
				{
					minY = porigon->position[2].y;
					maxY = porigon->position[1].y;
				}
			}
			else
			{
				if (porigon->position[1].y < porigon->position[2].y)
				{
					minY = porigon->position[1].y;
					maxY = porigon->position[0].y < porigon->position[2].y ?
						porigon->position[2].y : porigon->position[0].y;
				}
				else
				{
					minY = porigon->position[2].y;
					maxY = porigon->position[0].y;
				}
			}

			float minZ, maxZ;
			if (porigon->position[0].z < porigon->position[1].z)
			{
				if (porigon->position[0].z < porigon->position[2].z)
				{
					minZ = porigon->position[0].z;
					maxZ = porigon->position[1].z < porigon->position[2].z ?
						porigon->position[2].z : porigon->position[1].z;
				}
				else
				{
					minZ = porigon->position[2].z;
					maxZ = porigon->position[1].z;
				}
			}
			else
			{
				if (porigon->position[1].z < porigon->position[2].z)
				{
					minZ = porigon->position[1].z;
					maxZ = porigon->position[0].z < porigon->position[2].z ?
						porigon->position[2].z : porigon->position[0].z;
				}
				else
				{
					minZ = porigon->position[2].z;
					maxZ = porigon->position[0].z;
				}
			}

			// �o�^
			tree.Regist(minX, maxX, minY, maxY, minZ, maxZ, porigon);
		}
	}
}

bool MapCollider::ShpereVsPorigon(
	Collider*& other,
	DirectX::XMFLOAT3& direction,
	Porigon* porigon,
	HitResult& result
)
{
	DirectX::XMVECTOR Vertex[3] =
	{
		XMLoadFloat3(&porigon->position[0]),
		XMLoadFloat3(&porigon->position[1]),
		XMLoadFloat3(&porigon->position[2])
	};

	DirectX::XMVECTOR Edge[3] =
	{
		XMVectorSubtract(Vertex[1], Vertex[0]),
		XMVectorSubtract(Vertex[2], Vertex[1]),
		XMVectorSubtract(Vertex[0], Vertex[2])
	};

	// �ʖ@��
	XMVECTOR Normal = XMLoadFloat3(&porigon->normal);

	// ���ʂƈړ��O��̋��̋��������߂�
	XMFLOAT3 sphereCenter = other->GetPosition();
	XMFLOAT3 sphereCenterOri = other->GetPosition() + direction;
	XMVECTOR  SphereCenter = XMLoadFloat3(&sphereCenter);
	XMVECTOR  SphereCenterOri =  XMLoadFloat3(&sphereCenterOri);
	XMVECTOR  Dot1 = XMVector3Dot(Normal, SphereCenter);
	XMVECTOR  Dot2 = XMVector3Dot(Normal, SphereCenterOri);
	XMVECTOR  Dot3 = XMVector3Dot(Normal, Vertex[0]);
	XMVECTOR  Distance = XMVectorSubtract(Dot1, Dot3);
	XMVECTOR  DistanceOri = XMVectorSubtract(Dot2, Dot3);
	float distance = XMVectorGetX(Distance);
	float distanceOri = XMVectorGetX(DistanceOri);

	// �ړ��O�̋����ʂ̗����Ȃ瓖����Ȃ�
	if (distance < 0)	return false;

	// �ړ���̋����ʂ̕\���ł��̋��������a�ȏ�Ȃ瓖����Ȃ�
	float sphereRadius = other->GetScale().x; // ���̔��a
	if (distanceOri > sphereRadius)	return false;

	// �����O�p�`�����ɂ��邩
	bool outside = false;
	XMVECTOR Vec[3];
	for (int i = 0; i < 3; ++i)
	{
		Vec[i] = XMVectorSubtract(SphereCenter, Vertex[i]);
		XMVECTOR Cross = XMVector3Cross(Edge[i], Vec[i]);
		XMVECTOR Dot = XMVector3Dot(Normal, Cross);
		if (XMVectorGetX(Dot) < 0.0f)
		{
			outside = true;
		}
	}
	bool outsideOri = false;
	XMVECTOR VecOri[3];
	for (int i = 0; i < 3; ++i)
	{
		VecOri[i] = XMVectorSubtract(SphereCenterOri, Vertex[i]);
		XMVECTOR Cross = XMVector3Cross(Edge[i], VecOri[i]);
		XMVECTOR Dot = XMVector3Dot(Normal, Cross);
		if (XMVectorGetX(Dot) < 0.0f)
		{
			outsideOri = true;
		}
	}

	return false;
}

bool MapCollider::RayVsPorigon(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	Porigon* porigon,
	HitResult& result
)
{
	XMVECTOR Start = XMLoadFloat3(&start);
	XMVECTOR End = XMLoadFloat3(&end);
	XMVECTOR rayVec = XMVectorSubtract(End, Start);
	float length = XMVectorGetX(XMVector3Length(rayVec));

	// ���ς̌��ʂ��v���X�Ȃ�Η�����
	XMVECTOR N = XMLoadFloat3(&porigon->normal);
	float dot = XMVectorGetX(XMVector3Dot(rayVec, N));
	if (dot >= 0.0f)	return false;

	// ���C�̎n�_���畽�ʂ̌�_�܂ł̋������Z�o
	XMVECTOR A = XMLoadFloat3(&porigon->position[0]);
	XMVECTOR B = XMLoadFloat3(&porigon->position[1]);
	XMVECTOR C = XMLoadFloat3(&porigon->position[2]);
	XMVECTOR SA = XMVectorSubtract(A, Start);
	float x = XMVectorGetX(XMVector3Dot(SA, N)) / dot;
	float distance = length * x;

	// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������
	// �傫�����̓X�L�b�v
	if (distance < 0.0f || distance > result.distance) return false;

	// ���ʏ�̌�_P
	XMVECTOR P = XMVectorAdd(Start, XMVectorScale(rayVec, x));

	// ��_���O�p�`�̓����ɂ��邩����
	// �P��
	XMVECTOR Cross1 = XMVector3Cross(XMVectorSubtract(A, P), XMVectorSubtract(B, A));
	float Dot1 = XMVectorGetX(XMVector3Dot(rayVec, Cross1));
	if (Dot1 > 0.0f) return false;
	// �Q��
	XMVECTOR Cross2 = XMVector3Cross(XMVectorSubtract(B, P), XMVectorSubtract(C, B));
	float Dot2 = XMVectorGetX(XMVector3Dot(rayVec, Cross2));
	if (Dot2 > 0.0f) return false;
	// �R��
	XMVECTOR Cross3 = XMVector3Cross(XMVectorSubtract(C, P), XMVectorSubtract(A, C));
	float Dot3 = XMVectorGetX(XMVector3Dot(rayVec, Cross3));
	if (Dot3 > 0.0f) return false;

	// �q�b�g���ۑ�
	XMStoreFloat3(&result.position, P);
	result.normal = porigon->normal;
	result.distance = distance;
	result.materialIndex = porigon->materialIndex;

	return true;
}

bool MapCollider::SearchChildren(	// �q��ԒT��
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result,
	bool& hit
)
{
	// ��ԊO�Ȃ�return
	if (Elem < 0) return hit;

	// �q��Ԃ����݂���Ȃ�q��Ԃ�T��
	if ((Elem + 1) << 3 < tree.m_iCellNum)
	{
		int childElem = (Elem << 3) + 1;

		for (int i = 0; i < 8; i++)
		{
			if (SearchChildren(childElem + i, start, end, result, hit))
				break;
		}
	}

	// ���̋�Ԃ����݂��Ȃ��Ȃ�return
	if (!tree.ppCellAry[Elem])	return hit;

	// ���̋�Ԃɓo�^����Ă���|���S���Ƃ̓����蔻��
	Liner8TreeManager<Porigon>::OFT<Porigon>* oft = tree.ppCellAry[Elem]->pLatest;
	while (oft)
	{
		if (RayVsPorigon(start, end, oft->m_pObject, result))
		{
			hit = true;
		}

		oft = oft->m_pNext;
	}

	return hit;
}

bool MapCollider::SearchParent(		// �e��ԒT��
	int Elem,
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result,
	bool& hit
)
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

		Liner8TreeManager<Porigon>::OFT<Porigon>* oft = tree.ppCellAry[parentElem]->pLatest;
		while (oft)
		{
			if (RayVsPorigon(start, end, oft->m_pObject, result))
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