//! @file MapCollider.cpp
//! @note 

#include "TAKOEngine/Physics/Collider/MapCollider.h"
#include "TAKOEngine/Physics/Collider/SphereCollider.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

MapCollider::MapCollider(iModel* _model, uint16_t _objType, DirectX::XMFLOAT4X4* _transform) : Collider(_objType, _transform)
{
	model = _model;
	m_shapeType = COLLIDER_TYPE::MAP;
}

bool MapCollider::CollisionVsShpere(
	SphereCollider* other,
	DirectX::XMFLOAT3& direction,
	HitResult& result
)
{
	HitResultVector hit;
	if (Collision::IntersectSphereCastVsModel(other->GetPosition(), other->GetPosition() + direction, other->GetRadius(), model, &hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.position);
		result.distance = hit.distance;
		return true;
	}
	return false;

	//bool hit = false;
	//DirectX::XMFLOAT3 sphereCenterOri = other->GetPosition();
	//DirectX::XMFLOAT3 sphereCenter = other->GetPosition() + direction;

	//const ModelResource* resource = model->GetResource();
	//for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	//{
	//	// メッシュノード取得
	//	const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

	//	// 球体をワールド空間からローカル空間へ変換
	//	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
	//	DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

	//	DirectX::XMVECTOR SphereCenter = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenter), InverseWorldTransform);
	//	DirectX::XMVECTOR SphereCenterOri = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenterOri), InverseWorldTransform);
	//	float sphereRadius = other->GetScale().x; // 球体半径
	//	DirectX::XMVECTOR SphereRadius = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat(&sphereRadius), InverseWorldTransform);
	//	DirectX::XMStoreFloat(&sphereRadius, SphereRadius);

	//	// 三角形（面）との交差判定
	//	const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
	//	const std::vector<UINT> indices = mesh.indices;

	//	int materialIndex = -1;

	//	for (UINT i = 0; i < indices.size(); i += 3)
	//	{
	//		// 三角形の頂点を抽出
	//		//const ModelResource::Vertex& a = vertices.at(indices.at(i));
	//		//const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
	//		//const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));
	//		const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
	//		const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
	//		const ModelResource::Vertex& c = vertices.at(indices.at(i));
	//		// 三角形頂点
	//		DirectX::XMVECTOR TriangleVertex[3] =
	//		{
	//			DirectX::XMLoadFloat3(&a.position),
	//			DirectX::XMLoadFloat3(&b.position),
	//			DirectX::XMLoadFloat3(&c.position),
	//		};
	//		// 面法線
	//		DirectX::XMVECTOR Edge[3] =
	//		{
	//			DirectX::XMVectorSubtract(TriangleVertex[1], TriangleVertex[0]),
	//			DirectX::XMVectorSubtract(TriangleVertex[2], TriangleVertex[1]),
	//			DirectX::XMVectorSubtract(TriangleVertex[0], TriangleVertex[2]),
	//		};
	//		DirectX::XMVECTOR TriangleNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Edge[0], Edge[1]));

	//		DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(TriangleNormal, SphereCenter);
	//		DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(TriangleNormal, TriangleVertex[0]);
	//		DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(TriangleNormal, SphereCenterOri);
	//		DirectX::XMVECTOR Distance = DirectX::XMVectorSubtract(Dot1, Dot2);
	//		DirectX::XMVECTOR DistanceOri = DirectX::XMVectorSubtract(Dot3, Dot2);
	//		float distance = DirectX::XMVectorGetX(Distance);
	//		float distanceOri = DirectX::XMVectorGetX(DistanceOri);

	//		// 球の中心が面に接しているか、負の方向にある場合は当たらない
	//		if (distance < 0 && distanceOri < 0) continue;

	//		// 球が三角形内部に存在するか
	//		if (distance > sphereRadius) continue;

	//		bool outside = false;
	//		DirectX::XMVECTOR Vec[3];
	//		for (int i = 0; i < 3; ++i)
	//		{
	//			Vec[i] = DirectX::XMVectorSubtract(SphereCenter, TriangleVertex[i]);
	//			DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(Edge[i], Vec[i]);
	//			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(TriangleNormal, Cross);
	//			if (DirectX::XMVectorGetX(Dot) < 0.0f)
	//			{
	//				outside = true;
	//			}
	//		}
	//		// 三角形の内側なので交差する
	//		if (!outside)
	//		{
	//			materialIndex = mesh.materialIndex;

	//			float depth = sphereRadius - distance;
	//			DirectX::XMVECTOR Reflection = DirectX::XMVectorScale(TriangleNormal, depth);
	//			SphereCenter = DirectX::XMVectorAdd(SphereCenter, Reflection);

	//			continue;
	//		}
	//		// 外側
	//		// エッジとの判定
	//		const float radiusSq = sphereRadius * sphereRadius;
	//		for (int i = 0; i < 3; ++i)
	//		{
	//			// 辺の射影値を求める
	//			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec[i], Edge[i]));
	//			if (t > 0.0f)
	//			{
	//				// 辺の始点から終点までのベクトルと支店から球までのベクトルが同一の場合、
	//				// 内積値が辺の長さの2乗になる性質を利用して辺から球までの最短ベクトルを求める
	//				float edgeLengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Edge[i]));
	//				if (t > edgeLengthSq)
	//				{
	//					Vec[i] = DirectX::XMVectorSubtract(Vec[i], Edge[i]);
	//				}
	//				else
	//				{
	//					t /= edgeLengthSq;
	//					Vec[i] = DirectX::XMVectorSubtract(Vec[i], DirectX::XMVectorScale(Edge[i], t));
	//				}
	//			}
	//			// 辺から球までの最短ベクトルの距離が半径以下ならめり込んでいる
	//			float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec[i]));
	//			if (lengthSq < radiusSq)
	//			{
	//				materialIndex = mesh.materialIndex;
	//				// めり込み量算出
	//				float depth = sphereRadius - sqrtf(lengthSq);
	//				// めり込み分押し出し処理
	//				DirectX::XMVECTOR Reflection = DirectX::XMVector3Normalize(Vec[i]);
	//				Reflection = DirectX::XMVectorScale(Reflection, depth);
	//				SphereCenter = DirectX::XMVectorAdd(SphereCenter, Reflection);

	//				break;
	//			}
	//		}
	//	}
	//	if (materialIndex >= 0)
	//	{
	//		hit = true;
	//		result.materialIndex = materialIndex;

	//		// ローカル空間からワールド空間へ変換
	//		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(SphereCenter, WorldTransform);

	//		DirectX::XMStoreFloat3(&sphereCenter, WorldPosition);
	//	}
	//}

	//if (hit)
	//{
	//	result.position = sphereCenter;
	//	result.normal = sphereCenter - other->GetPosition();
	//	result.distance = XMFLOAT3Length(result.normal);
	//	if (result.distance > 0.0f)
	//	{
	//		result.normal /= result.distance;
	//	}
	//}

	//return hit;
}

bool MapCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	HitResultVector hit;
	if (Collision::IntersectRayVsModel(start, end, model, hit))
	{
		XMStoreFloat3(&result.normal, hit.normal);
		XMStoreFloat3(&result.position, hit.position);
		result.distance = hit.distance;
		XMStoreFloat3(&result.triangleVerts[0], hit.triangleVerts[0]);
		XMStoreFloat3(&result.triangleVerts[1], hit.triangleVerts[1]);
		XMStoreFloat3(&result.triangleVerts[2], hit.triangleVerts[2]);
		return true;
	}
	return false;
}