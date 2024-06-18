#include "TAKOEngine/Physics/ModelCollider.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

ModelCollider::ModelCollider(Model* model)
{
	this->model = model;

	type = COLLIDER_TYPE::MODEL;
}

bool ModelCollider::CollisionVsShpere(
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
		// メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// 球体をワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR SphereCenter = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenter), InverseWorldTransform);
		DirectX::XMVECTOR SphereCenterOri = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenterOri), InverseWorldTransform);
		float sphereRadius = other->GetScale().x; // 球体半径

		// 三角形（面）との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;

		for (UINT i = 0; i < indices.size(); i += 3)
		{
			// 三角形の頂点を抽出
			const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			const ModelResource::Vertex& c = vertices.at(indices.at(i));
			// 三角形頂点
			DirectX::XMVECTOR TriangleVertex[3] =
			{
				DirectX::XMLoadFloat3(&a.position),
				DirectX::XMLoadFloat3(&b.position),
				DirectX::XMLoadFloat3(&c.position),
			};
			// 面法線
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

			// 球の中心が面に接しているか、負の方向にある場合は当たらない
			if (distance < 0 && distanceOri < 0) continue;

			// 球が三角形内部に存在するか
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
			// 三角形の内側なので交差する
			if (!outside)
			{
				materialIndex = mesh.materialIndex;

				float depth = sphereRadius - distance;
				DirectX::XMVECTOR Reflection = DirectX::XMVectorScale(TriangleNormal, depth);
				SphereCenter = DirectX::XMVectorAdd(SphereCenter, Reflection);

				continue;
			}
			// 外側
			// エッジとの判定
			const float radiusSq = sphereRadius * sphereRadius;
			for (int i = 0; i < 3; ++i)
			{
				// 辺の射影値を求める
				float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec[i], Edge[i]));
				if (t > 0.0f)
				{
					// 辺の始点から終点までのベクトルと支店から球までのベクトルが同一の場合、
					// 内積値が辺の長さの2乗になる性質を利用して辺から球までの最短ベクトルを求める
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
				// 辺から球までの最短ベクトルの距離が半径以下ならめり込んでいる
				float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec[i]));
				if (lengthSq < radiusSq)
				{
					materialIndex = mesh.materialIndex;
					// めり込み量算出
					float depth = sphereRadius - sqrtf(lengthSq);
					// めり込み分押し出し処理
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

			// ローカル空間からワールド空間へ変換
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

bool ModelCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
	// ワールド空間のレイの長さ
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

		// レイの長さ
		float neart;
		DirectX::XMStoreFloat(&neart, Length);

		// 三角形（面）との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		int materialIndex = -1;

		for (UINT i = 0; i < indices.size(); i += 3)
		{
			// 三角形の頂点を抽出
			const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
			const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			const ModelResource::Vertex& c = vertices.at(indices.at(i));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// 三角形の三辺ベクトルを算出
			DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
			DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
			DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

			// 三角形の法線ベクトルを算出
			DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

			// 内積の結果がプラスならば裏向き
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
			if (DirectX::XMVectorGetX(Dot) >= 0.0f) continue;

			// レイと平面の交点を算出
			DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
			DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
			float x;
			DirectX::XMStoreFloat(&x, X);
			// 交点までの距離が今までに計算した最近距離より
			// 大きい時はスキップ
			if (x < 0.0f || x > neart) continue;

			DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

			// 交点が三角形の内側にあるか判定
			// １つめ
			DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
			DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
			DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(V, Cross1);
			if (DirectX::XMVectorGetX(Dot1) > 0.0f) continue;
			// ２つめ
			DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
			DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
			DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(V, Cross2);
			if (DirectX::XMVectorGetX(Dot2) > 0.0f) continue;
			// ３つめ
			DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
			DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
			DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(V, Cross3);
			if (DirectX::XMVectorGetX(Dot3) > 0.0f) continue;

			// 最近距離を更新
			neart = x;
			// 交点と法線を更新
			HitPosition = P;
			HitNormal = N;
			materialIndex = mesh.materialIndex;
		}

		if (materialIndex >= 0)
		{
			// ローカル空間からワールド空間へ変換
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);
			// ヒット情報保存
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}

	return hit;
}