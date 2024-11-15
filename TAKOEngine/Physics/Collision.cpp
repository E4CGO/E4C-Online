//! @file Collision.cpp
//! @note 当たり判定処理総まとめ

#include "Collision.h"

#undef min
#undef max

// 球Vs球
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMVECTOR& position1,
	float radius1,
	const DirectX::XMVECTOR& position2,
	float radius2,
	IntersectionResult* result)
{
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(position1, position2);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radius1 + radius2;
	if (lengthSq > range * range)
	{
		return false;
	}

	if (result)
	{
		result->normal = DirectX::XMVector3Normalize(vec);
		result->penetration = range - sqrtf(lengthSq);
		result->pointA = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(result->normal, -radius1));
		result->pointB = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(result->normal, radius2));
	}

	return true;
}

// レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const iModel* model,
	HitResultVector& result)
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
		const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

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

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		DirectX::XMVECTOR HitVerts[3] = {};
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

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
				float d;
				DirectX::XMStoreFloat(&d, Dot);
				if (d >= 0) continue;

				// レイと平面の交点を算出
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(N, SA), Dot);
				float x;
				DirectX::XMStoreFloat(&x, X);
				if (x < .0f || x > neart) continue;	// 交点までの距離が今までに計算した最近距離より
				// 大きい時はスキップ
				DirectX::XMVECTOR P = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(V, X), S);

				// 交点が三角形の内側にあるか判定
				// １つめ
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
				float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f) continue;
				// ２つめ
				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
				float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f) continue;
				// ３つめ
				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
				float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f) continue;

				// 三角形頂点を更新
				HitVerts[0] = A;
				HitVerts[1] = B;
				HitVerts[2] = C;

				// 最近距離を更新
				neart = x;

				// 交点と法線を更新
				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
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
				result.position = WorldPosition;
				result.normal = DirectX::XMVector3Normalize(WorldNormal);

				for (int i = 0; i < 3; ++i)
				{
					DirectX::XMVECTOR Vertex = DirectX::XMVector3Transform(HitVerts[i], WorldTransform);
					result.triangleVerts[i] = Vertex;
				}

				hit = true;
			}
		}
	}

	return hit;
}

// レイと三角形の交差判定
bool Collision::IntersectRayVsTriangle(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMVECTOR triangleVerts[3],
	HitResultVector& result)
{
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(triangleVerts[1], triangleVerts[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(triangleVerts[2], triangleVerts[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, triangleVerts[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						result.position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result.distance));
						result.normal = DirectX::XMVector3Normalize(norm);
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 非正規化
	const DirectX::XMVECTOR triangleVerts[3],
	HitResultVector& result)
{
	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(triangleVerts[1], triangleVerts[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(triangleVerts[2], triangleVerts[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStart, DirectX::XMVectorAdd(rayStart, rayDirection));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStart, triangleVerts[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						DirectX::XMVECTOR tmp = DirectX::XMVectorScale(rayDirection, t / d);
						result.distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(tmp));
						result.position = DirectX::XMVectorAdd(rayStart, tmp);
						result.normal = DirectX::XMVector3Normalize(norm);
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMFLOAT3& rayStart,
	const DirectX::XMFLOAT3& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMFLOAT3 triangleVerts[3],
	HitResult& result)
{
	DirectX::XMVECTOR rayStartVec = DirectX::XMLoadFloat3(&rayStart);
	DirectX::XMVECTOR rayDirectionVec = DirectX::XMLoadFloat3(&rayDirection);
	DirectX::XMVECTOR trianglePos[3] = {};
	for (int i = 0; i < 3; i++)
	{
		trianglePos[i] = DirectX::XMLoadFloat3(&triangleVerts[i]);
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStartVec, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStartVec, trianglePos[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						DirectX::XMStoreFloat3(&result.position, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, result.distance)));
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(norm));
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

// 外部の点に対する三角形内部の最近点を算出する
// 返り値：nearPosが三角形内部（辺や頂点でない）の時true
bool Collision::GetClosestPoint_PointTriangle(
	const DirectX::XMVECTOR& point,				// 比較座標
	const DirectX::XMVECTOR trianglePos[3],		// 三角形の頂点	
	DirectX::XMVECTOR& nearPos)					// 三角形側の最近点
{
	// pointがTrianglePos[0]の外側の頂点領域にあるかチェック
	DirectX::XMVECTOR Vec01 = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR Vec02 = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR Vec0P = DirectX::XMVectorSubtract(point, trianglePos[0]);
	float d1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec0P));
	float d2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec0P));
	if (d1 < 0 && d2 < 0)
	{
		nearPos = trianglePos[0];
		return false;
	}

	// pointがTrianglePos[1]の外側の頂点領域にあるかチェック
	DirectX::XMVECTOR Vec1P = DirectX::XMVectorSubtract(point, trianglePos[1]);
	float d3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec1P));
	float d4 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec1P));
	if (d3 > 0 && d4 < d3)
	{
		nearPos = trianglePos[1];
		return false;
	}

	// pointがTrianglePos[2]の外側の頂点領域にあるかチェック
	DirectX::XMVECTOR Vec2P = DirectX::XMVectorSubtract(point, trianglePos[2]);
	float d5 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec01, Vec2P));
	float d6 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Vec02, Vec2P));
	if (d6 > 0 && d5 < d6)
	{
		nearPos = trianglePos[2];
		return false;
	}

	// pointがTrianglePos[0]とTrianglePos[1]の外側の辺領域にあるかチェック
	float v = d1 * d4 - d3 * d2;
	if (v < 0)
	{
		if (d1 >= 0.0f && d3 <= 0.0f)
		{
			float t = d1 / (d1 - d3);
			nearPos = DirectX::XMVectorAdd(trianglePos[0], DirectX::XMVectorScale(Vec01, t));
			return false;
		}
	}

	// pointがTrianglePos[0]とTrianglePos[2]の外側の辺領域にあるかチェック
	float u = d5 * d2 - d1 * d6;
	if (u < 0)
	{
		if (d2 >= 0.0f && d6 <= 0.0f)
		{
			float t = d2 / (d2 - d6);
			nearPos = DirectX::XMVectorAdd(trianglePos[0], DirectX::XMVectorScale(Vec02, t));
			return false;
		}
	}

	// pointがTrianglePos[1]とTrianglePos[2]の外側の辺領域にあるかチェック
	float w = d3 * d6 - d5 * d4;
	if (w < 0)
	{
		if (d4 >= d3 && d5 >= d6)
		{
			float t = (d4 - d3) / ((d4 - d3) - (d6 - d5));
			nearPos = DirectX::XMVectorAdd(trianglePos[1],
				DirectX::XMVectorScale(DirectX::XMVectorSubtract(trianglePos[2], trianglePos[1]), t));
			return false;
		}
	}

	// ここまでくれば、nearPosは三角形の内部にある
	float t01 = u / (w + u + v);
	float t02 = v / (w + u + v);

	nearPos = DirectX::XMVectorAdd(trianglePos[0],
		DirectX::XMVectorAdd(DirectX::XMVectorScale(Vec01, t01), DirectX::XMVectorScale(Vec02, t02)));

	return true;
}

// 球Vs三角形
bool Collision::IntersectSphereVsTriangle(
	const DirectX::XMVECTOR& spherePos,
	float radius,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result,
	bool triangleNormOnly)
{
	DirectX::XMVECTOR tmpPos = {};
	GetClosestPoint_PointTriangle(spherePos, trianglePos, tmpPos);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(spherePos, tmpPos);
	bool ret = false;
	if (result)
	{
		DirectX::XMVECTOR vecNorm = DirectX::XMVector3Normalize(vec);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(vecNorm, radius));
		result->pointB = tmpPos;
		ret = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec)) <= (radius * radius);

		if (ret)
		{
			ret = true;
		}
		if (ret && triangleNormOnly)
		{
			result->normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]), DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0])));
			float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(result->normal, vec));
			result->penetration = radius - dist;
		}
		else
		{
			result->normal = vecNorm;
			result->penetration = radius - DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
		}
	}
	return ret;
}

// 球vsAABBの交差判定
bool Collision::IntersectSphereVsAABB(
	const DirectX::XMVECTOR& spherePos,
	float sphereRadius,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	IntersectionResult* result)
{
	DirectX::XMFLOAT3 aabbPosF3 = {};
	DirectX::XMStoreFloat3(&aabbPosF3, aabbPos);
	DirectX::XMFLOAT3 aabbRadiiF3 = {};
	DirectX::XMStoreFloat3(&aabbRadiiF3, aabbRadii);
	DirectX::XMFLOAT3 hitPos = {};
	DirectX::XMStoreFloat3(&hitPos, spherePos);

	if (hitPos.x < aabbPosF3.x - aabbRadiiF3.x)
	{
		hitPos.x = aabbPosF3.x - aabbRadiiF3.x;
	}
	else if (hitPos.x > aabbPosF3.x + aabbRadiiF3.x)
	{
		hitPos.x = aabbPosF3.x + aabbRadiiF3.x;
	}

	if (hitPos.y < aabbPosF3.y - aabbRadiiF3.y)
	{
		hitPos.y = aabbPosF3.y - aabbRadiiF3.y;
	}
	else if (hitPos.y > aabbPosF3.y + aabbRadiiF3.y)
	{
		hitPos.y = aabbPosF3.y + aabbRadiiF3.y;
	}

	if (hitPos.z < aabbPosF3.z - aabbRadiiF3.z)
	{
		hitPos.z = aabbPosF3.z - aabbRadiiF3.z;
	}
	else if (hitPos.z > aabbPosF3.z + aabbRadiiF3.z)
	{
		hitPos.z = aabbPosF3.z + aabbRadiiF3.z;
	}

	// 距離判定
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(spherePos, DirectX::XMLoadFloat3(&hitPos));
	float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

	if (result)
	{
		result->penetration = sphereRadius - sqrtf(lengthSq);
		result->pointB = DirectX::XMLoadFloat3(&hitPos);
		result->normal = DirectX::XMVector3Normalize(Vec);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result->normal, -sphereRadius));
	}

	return (lengthSq < sphereRadius * sphereRadius);
}

// 球vsカプセルの交差判定
bool Collision::IntersectSphereVsCapsule(
	const DirectX::XMVECTOR& spherePos,
	float sphereRadius,
	const DirectX::XMVECTOR& capsulePos,
	const DirectX::XMVECTOR& capsuleDirection,
	float capsuleLength,
	float capsuleRadius,					// カプセルの半径
	IntersectionResult* result)
{
	float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(capsuleDirection, DirectX::XMVectorSubtract(spherePos, capsulePos)));	// 射影長の算出
	DirectX::XMVECTOR point = {};	// カプセル中心線上の最近点
	if (t < 0)
	{
		point = capsulePos;
	}
	else if (t > capsuleLength)
	{
		point = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(capsuleDirection, capsuleLength));
	}
	else
	{
		point = DirectX::XMVectorAdd(capsulePos, DirectX::XMVectorScale(capsuleDirection, t));
	}

	// 交差判定
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(spherePos, point);
	float radiusAdd = sphereRadius + capsuleRadius;
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));

	if (result)
	{
		result->normal = DirectX::XMVector3Normalize(vec);
		result->penetration = radiusAdd - sqrtf(distSq);
		result->pointA = DirectX::XMVectorAdd(spherePos, DirectX::XMVectorScale(result->normal, -sphereRadius));
		result->pointB = DirectX::XMVectorAdd(point, DirectX::XMVectorScale(result->normal, capsuleRadius));
	}

	return distSq < radiusAdd * radiusAdd;
}

// レイVsスラブ3D (軸平行スラブ)
bool Collision::IntersectRayVsSlub3D(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMVECTOR& slubCenter,
	const DirectX::XMVECTOR& slubRadii,
	HitResultVector* resultNear,		// レイとスラブの最初の交点情報
	HitResultVector* resultFar)		// レイとスラブの最後の交点情報
{
	// ループ処理するため、xyz成分を配列に代入する
	float startPosArray[3] = { DirectX::XMVectorGetX(rayStart), DirectX::XMVectorGetY(rayStart), DirectX::XMVectorGetZ(rayStart) };
	float slubCenterArray[3] = { DirectX::XMVectorGetX(slubCenter), DirectX::XMVectorGetY(slubCenter), DirectX::XMVectorGetZ(slubCenter) };
	float slubRadiiArray[3] = { DirectX::XMVectorGetX(slubRadii), DirectX::XMVectorGetY(slubRadii), DirectX::XMVectorGetZ(slubRadii) };
	float dArray[3] = { DirectX::XMVectorGetX(rayDirection), DirectX::XMVectorGetY(rayDirection), DirectX::XMVectorGetZ(rayDirection) };

	// 直線とスラブの２交点までの距離をtminとtmaxと定義
	float tmin = 0.0f;
	float tmax = FLT_MAX;
	int minAxis = 0;

	bool ret = true;

	// スラブとの距離を算出し交差しているかの確認と最近点の算出を行う
	for (int i = 0; i < 3; i++)
	{
		//xyz軸との平行確認
		if (fabsf(dArray[i]) < FLT_EPSILON)
		{
			// 平行の場合、位置関係の比較を行い範囲内になければ交差なし
			if (startPosArray[i] < slubCenterArray[i] - slubRadiiArray[i] || startPosArray[i] > slubCenterArray[i] + slubRadiiArray[i])
			{
				ret = false;
			}
		}
		else
		{
			// t1が近スラブ、t2が遠スラブとの距離
			float ood = 1.0f / dArray[i];
			float t1 = (slubCenterArray[i] - slubRadiiArray[i] - startPosArray[i]) * ood;
			float t2 = (slubCenterArray[i] + slubRadiiArray[i] - startPosArray[i]) * ood;

			// 遠近が逆転している場合があるので、その場合入れ替えておく
			if (t1 > t2)
			{
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}

			// t1がtminよりも大きい場合、tminをt1で更新する
			if (t1 > tmin)
			{
				tmin = t1;
				minAxis = i;
			}

			// t2がtmaxよりも小さい場合、tmaxをt2で更新する
			if (t2 < tmax)
			{
				tmax = t2;
			}
		}
	}

	if (resultNear)
	{
		resultNear->distance = tmin;
		resultNear->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultNear->distance));
		DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
		if (dArray[minAxis] > 0)
		{
			norm = DirectX::XMVectorNegate(norm);
		}
		resultNear->normal = norm;

		if (resultFar)
		{
			resultFar->distance = tmax;
			resultFar->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, resultFar->distance));
			DirectX::XMVECTOR norm = { static_cast<float>(minAxis == 0),   static_cast<float>(minAxis == 1),  static_cast<float>(minAxis == 2) };
			if (dArray[minAxis] > 0)
			{
				norm = DirectX::XMVectorNegate(norm);
			}
			resultFar->normal = norm;
		}
	}

	return ret && (rayDist >= tmin);
}

// レイVsAABB
bool Collision::IntersectRayVsAABB(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	HitResultVector* resultNear,
	HitResultVector* resultFar)
{
	HitResultVector nearPoint = {};
	HitResultVector farPoint = {};
	if (IntersectRayVsSlub3D(rayStart, rayDirection, rayDist, aabbPos, aabbRadii, &nearPoint, &farPoint))
	{
		if (nearPoint.distance < farPoint.distance)
		{
			if (resultNear)
			{
				*resultNear = nearPoint;
				if (resultFar)
				{
					*resultFar = farPoint;
				}
			}
			return true;
		}
	}

	return false;
}

// 外部の点に対するAABB内部の最近点を取得する
DirectX::XMVECTOR Collision::GetClosestPoint_PointAABB(
	const DirectX::XMVECTOR& point,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	bool surfaceFlg)
{
	float aabbPosArray[3] = { DirectX::XMVectorGetX(aabbPos), DirectX::XMVectorGetY(aabbPos), DirectX::XMVectorGetZ(aabbPos) };
	float aabbRadArray[3] = { DirectX::XMVectorGetX(aabbRadii), DirectX::XMVectorGetY(aabbRadii), DirectX::XMVectorGetZ(aabbRadii) };
	float nearPoint[3] = { DirectX::XMVectorGetX(point), DirectX::XMVectorGetY(point), DirectX::XMVectorGetZ(point) };

	int targetAxis = -1;
	int count = 0;
	float dist = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		if (nearPoint[i] < aabbPosArray[i] - aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] - aabbRadArray[i];
		}
		else if (nearPoint[i] > aabbPosArray[i] + aabbRadArray[i])
		{
			nearPoint[i] = aabbPosArray[i] + aabbRadArray[i];
		}
		else if (nearPoint[i] <= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] - aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] - aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
		else if (nearPoint[i] >= aabbPosArray[i])
		{
			if (fabsf(dist) > fabsf(aabbPosArray[i] + aabbRadArray[i] - nearPoint[i]))
			{
				targetAxis = i;
				dist = aabbPosArray[i] + aabbRadArray[i] - nearPoint[i];
			}
			count++;
		}
	}

	// surfaceFlgがtrueかつcountが3の場合、内部にあるため、最近点を表面に補正する
	if (surfaceFlg)
	{
		if (count == 3)
		{
			if (nearPoint[targetAxis] < aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] - aabbRadArray[targetAxis];
			}
			else if (nearPoint[targetAxis] > aabbPosArray[targetAxis])
			{
				nearPoint[targetAxis] = aabbPosArray[targetAxis] + aabbRadArray[targetAxis];
			}
		}
	}

	return DirectX::XMVECTOR{ nearPoint[0], nearPoint[1], nearPoint[2] };
}

// スフィアキャストorカプセルVsAABB用の頂点算出関数
inline DirectX::XMVECTOR GetAABBCorner(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int v)
{
	DirectX::XMFLOAT3 p = {};
	DirectX::XMStoreFloat3(&p, aabbPos);
	p.x += (v & (1 << 0)) ? DirectX::XMVectorGetX(aabbRadii) : -DirectX::XMVectorGetX(aabbRadii);
	p.y += (v & (1 << 1)) ? DirectX::XMVectorGetY(aabbRadii) : -DirectX::XMVectorGetY(aabbRadii);
	p.z += (v & (1 << 2)) ? DirectX::XMVectorGetZ(aabbRadii) : -DirectX::XMVectorGetZ(aabbRadii);

	return DirectX::XMLoadFloat3(&p);
}

// 線分とAABBの最短距離について、面領域に線分の端点がある場合の処理
inline float GetMinDistSq_SegmentAABBFace_PointInFaceArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	DirectX::XMVECTOR nearTargetPoint = Collision::GetClosestPoint_PointAABB(targetPoint, aabbPos, aabbRadii);
	float minDist = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(targetPoint, nearTargetPoint)));
	if (nearPointSegment) *nearPointSegment = targetPoint;
	if (nearPointSegment) *nearPointAABB = nearTargetPoint;

	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
	}
	else if (v == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}
	else if (v == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}


	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// 線分とAABBの最短距離について、頂点領域に線分の端点がある場合の処理
inline float GetMinDistSq_SegmentAABBFace_PointInCornerArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 7 && v == 0)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
	}
	else if (u == 6 && v == 1)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (u == 5 && v == 2)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 4 && v == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
	}
	else if (u == 3 && v == 4)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
	}
	else if (u == 2 && v == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
	}
	else if (u == 1 && v == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
	}
	else if (u == 0 && v == 7)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}

	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float minDist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[0], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[5], aabbFaceCorner[6], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// 線分とAABBの最短距離について、辺領域に線分の端点がある場合の処理
inline float GetMinDistSq_SegmentAABBFace_PointInEdgeArea(
	const  DirectX::XMVECTOR& targetPoint,
	const  DirectX::XMVECTOR& otherPoint,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const int u,
	const int v,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	std::vector<DirectX::XMVECTOR> aabbFaceCorner;
	if (u == 1)
	{
		if (v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		}
		else if (v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		}
	}
	else if (u == 2)
	{
		if (v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		}
		else if (v == 4)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
	}
	else if (u == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}
	else if (u == 4)
	{
		if (v == 1)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
		else if (v == 2)
		{
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
			aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		}
	}
	else if (u == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
	}
	else if (u == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 0));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 3)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}
	else if (v == 5)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 1));
	}
	else if (v == 6)
	{
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 5));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 7));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 3));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 4));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 6));
		aabbFaceCorner.push_back(GetAABBCorner(aabbPos, aabbRadii, 2));
	}


	DirectX::XMVECTOR tmpNearPointSegment = {};
	DirectX::XMVECTOR tmpNearPointAABB = {};
	float minDist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[1], nearPointSegment, nearPointAABB);
	float dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[2], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[0], aabbFaceCorner[3], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[1], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[2], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[3], aabbFaceCorner[4], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}
	dist = Collision::GetMinDistSq_SegmentSegment(targetPoint, otherPoint, aabbFaceCorner[4], aabbFaceCorner[5], &tmpNearPointSegment, &tmpNearPointAABB);
	if (dist < minDist)
	{
		minDist = dist;
		if (nearPointSegment) *nearPointSegment = tmpNearPointSegment;
		if (nearPointSegment) *nearPointAABB = tmpNearPointAABB;
	}

	return minDist;
}

// 線分とAABBの最短距離の二乗を取得する
float Collision::GetMinDistSq_SegmentAABB(
	const  DirectX::XMVECTOR& pointA,
	const  DirectX::XMVECTOR& pointB,
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	DirectX::XMVECTOR* nearPointSegment,
	DirectX::XMVECTOR* nearPointAABB)
{
	// 線分をレイと見立ててAABBと交差するか確認
	DirectX::XMVECTOR rayDirection = DirectX::XMVectorSubtract(pointB, pointA);
	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(rayDirection));
	rayDirection = DirectX::XMVector3Normalize(rayDirection);
	HitResultVector tmpResultNear = {}, tmpResultFar = {};
	float minDistanceSq = 0.0f;

	if (IntersectRayVsAABB(pointA, rayDirection, rayDist, aabbPos, aabbRadii, &tmpResultNear, &tmpResultFar))
	{
		if (nearPointSegment)
		{
			if (tmpResultNear.distance == 0.0f)
			{
				*nearPointSegment = pointA;
			}
			else
			{
				*nearPointSegment = pointB;
			}
		}
		if (nearPointAABB)
		{
			if (tmpResultNear.distance == 0.0f)
			{
				*nearPointAABB = tmpResultFar.position;
			}
			else
			{
				*nearPointAABB = tmpResultNear.position;
			}
		}

		return 0.0f;
	}
	else
	{
		int uA = 0, vA = 0;
		if (DirectX::XMVectorGetX(pointA) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uA |= (1 << 0);
		else if (DirectX::XMVectorGetX(pointA) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vA |= (1 << 0);
		if (DirectX::XMVectorGetY(pointA) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uA |= (1 << 1);
		else if (DirectX::XMVectorGetY(pointA) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vA |= (1 << 1);
		if (DirectX::XMVectorGetZ(pointA) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uA |= (1 << 2);
		else if (DirectX::XMVectorGetZ(pointA) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vA |= (1 << 2);
		int maskA = uA | vA;

		int uB = 0, vB = 0;
		if (DirectX::XMVectorGetX(pointB) <= DirectX::XMVectorGetX(aabbPos) - DirectX::XMVectorGetX(aabbRadii))			uB |= (1 << 0);
		else if (DirectX::XMVectorGetX(pointB) >= DirectX::XMVectorGetX(aabbPos) + DirectX::XMVectorGetX(aabbRadii))	vB |= (1 << 0);
		if (DirectX::XMVectorGetY(pointB) <= DirectX::XMVectorGetY(aabbPos) - DirectX::XMVectorGetY(aabbRadii))			uB |= (1 << 1);
		else if (DirectX::XMVectorGetY(pointB) >= DirectX::XMVectorGetY(aabbPos) + DirectX::XMVectorGetY(aabbRadii))	vB |= (1 << 1);
		if (DirectX::XMVectorGetZ(pointB) <= DirectX::XMVectorGetZ(aabbPos) - DirectX::XMVectorGetZ(aabbRadii))			uB |= (1 << 2);
		else if (DirectX::XMVectorGetZ(pointB) >= DirectX::XMVectorGetZ(aabbPos) + DirectX::XMVectorGetZ(aabbRadii))	vB |= (1 << 2);
		int maskB = uB | vB;

		// 同じ領域の場合
		if (uA == uB && vA == vB)
		{
			DirectX::XMVECTOR nearPointA = GetClosestPoint_PointAABB(pointA, aabbPos, aabbRadii);
			DirectX::XMVECTOR nearPointB = GetClosestPoint_PointAABB(pointB, aabbPos, aabbRadii);
			float nearPointDistA = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointA, nearPointA)));
			float nearPointDistB = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(pointB, nearPointB)));

			DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vA);
			DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uA);
			float edgeDist = GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);

			float ret = 0.0f;
			if (nearPointDistA < nearPointDistB)
			{
				if (edgeDist < nearPointDistA)
				{
					ret = edgeDist;
				}
				else
				{
					ret = nearPointDistA;
					if (nearPointSegment) *nearPointSegment = pointA;
					if (nearPointAABB) *nearPointAABB = nearPointA;
				}
			}
			else
			{
				if (edgeDist < nearPointDistB)
				{
					ret = edgeDist;
				}
				else
				{
					ret = nearPointDistB;
					if (nearPointSegment) *nearPointSegment = pointB;
					if (nearPointAABB) *nearPointAABB = nearPointB;
				}
			}

			return ret;
		}
		else
		{
			// それぞれ違う面領域の場合
			if ((maskA & (maskA - 1)) == 0 && (maskB & (maskB - 1)) == 0)
			{
				int uAdd = uA + uB;
				int vAdd = vA + vB;
				DirectX::XMVECTOR cornerA = GetAABBCorner(aabbPos, aabbRadii, vAdd);
				DirectX::XMVECTOR cornerB = GetAABBCorner(aabbPos, aabbRadii, 7 - uAdd);

				return GetMinDistSq_SegmentSegment(pointA, pointB, cornerA, cornerB, nearPointSegment, nearPointAABB);
			}
			// pointA側のみが面領域の場合
			else if ((maskA & (maskA - 1)) == 0)
			{
				return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// pointB側のみが面領域の場合
			else if ((maskB & (maskB - 1)) == 0)
			{
				return GetMinDistSq_SegmentAABBFace_PointInFaceArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
			}
			// 両方が頂点領域の場合
			else if (maskA == 7 && maskB == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInCornerArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// pointA側のみが頂点領域の場合
			else if (maskA == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointB, pointA, aabbPos, aabbRadii, uB, vB, nearPointSegment, nearPointAABB);
			}
			// pointB側のみが頂点領域の場合
			else if (maskB == 7)
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
			// 両方が辺領域の場合
			else
			{
				return GetMinDistSq_SegmentAABBFace_PointInEdgeArea(pointA, pointB, aabbPos, aabbRadii, uA, vA, nearPointSegment, nearPointAABB);
			}
		}
	}
}

// カプセルvsAABBの交差判定
bool Collision::IntersectCapsuleVsAABB(
	const DirectX::XMVECTOR& position,	// 中心
	const DirectX::XMVECTOR& direction,	// 向き（正規化）
	const float				length,	// 長さ
	const float				radius,	// 半径
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	IntersectionResult* result)
{
	float dist = 0.0f;
	DirectX::XMVECTOR nearPointSegment = {};
	DirectX::XMVECTOR nearPointAABB = {};

	DirectX::XMVECTOR end = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, length));

	if (result)
	{
		dist = GetMinDistSq_SegmentAABB(position, end, aabbPos, aabbRadii, &nearPointSegment, &nearPointAABB);
	}
	else
	{
		dist = GetMinDistSq_SegmentAABB(position, end, aabbPos, aabbRadii);
	}

	if (dist < radius * radius)
	{
		if (result)
		{
			if (dist == 0.0f)
			{
				result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(nearPointAABB, nearPointSegment));
				result->pointA = DirectX::XMVectorAdd(nearPointSegment, DirectX::XMVectorScale(result->normal, -radius));
				result->pointB = nearPointAABB;
				result->penetration = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(result->pointB, result->pointA)));
			}
			else
			{
				result->penetration = radius - sqrtf(dist);
				result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(nearPointSegment, nearPointAABB));
				result->pointA = DirectX::XMVectorAdd(nearPointSegment, DirectX::XMVectorScale(result->normal, -radius));
				result->pointB = nearPointAABB;
			}
		}
		return true;
	}

	return false;
}

// レイVs球
bool Collision::IntersectRayVsSphere(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMVECTOR& spherePos,
	float radius,
	HitResultVector* result)
{
	DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(spherePos, rayStart);
	float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, rayDirection));
	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;

	if (distSq < radius * radius)
	{
		float distance = projection - sqrtf(radius * radius - distSq);
		if (distance > 0.0f)
		{
			if (distance < rayDist)
			{
				if (result)
				{
					result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, distance));
					result->distance = distance;
					result->normal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(result->position, spherePos));
				}
				return true;
			}
		}
	}

	return false;
}

// レイVs円柱
bool Collision::IntersectRayVsOrientedCylinder(
	const DirectX::XMVECTOR& rayStart,
	const DirectX::XMVECTOR& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMVECTOR& startCylinder,
	const DirectX::XMVECTOR& endCylinder,
	float radius,
	HitResultVector* result,
	DirectX::XMVECTOR* onCenterLinPos)
{
	DirectX::XMVECTOR d = DirectX::XMVectorSubtract(endCylinder, startCylinder);
	DirectX::XMVECTOR m = DirectX::XMVectorSubtract(rayStart, startCylinder);
	DirectX::XMVECTOR n = DirectX::XMVectorScale(rayDirection, rayDist);

	float md = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, d));
	float nd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
	float dd = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, d));

	// 線分全体が円柱の底面・上面に垂直なスラブに対して外側にあるかどうかを判定
	if (md < 0.0f && md + nd < 0.0f)	return false;
	if (md > dd && md + nd > dd)	return false;

	float nn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, n));
	float a = nn * dd - nd * nd;
	float mm = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, m));
	float k = mm - radius * radius;
	//float c = mm * dd - md * md - radius * radius * dd;
	float c = k * dd - md * md;

	// 線分が円柱の軸に対して平行
	if (fabsf(a) < 0.0001f)		// 誤差が出やすい計算なので閾値は大きめ（0.0001f）
	{
		if (c > 0.0f) return false;	// 線分は円柱の外側

		if (result)
		{
			// 底面交差のresultを算出
			if (md < 0)
			{
				result->distance = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, rayDirection));
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result->distance));
				result->normal = DirectX::XMVectorNegate(rayDirection);
			}
			// 上面交差のresultを算出
			else
			{
				result->distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(endCylinder, rayStart), rayDirection));
				result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result->distance));
				result->normal = DirectX::XMVectorNegate(rayDirection);
			}
		}
		return true;
	}

	// 線分が円柱の軸に対して平行でない
	// 円柱の表面を表す陰関数方程式と直線の方程式の解を求めて交差判定を行う。
	float mn = DirectX::XMVectorGetX(DirectX::XMVector3Dot(m, n));
	float b = mn * dd - nd * md;
	float D = b * b - a * c;	// 判別式

	if (D < 0) return false;	// 実数解がないので交差していない

	// 解の公式により、交点までの距離を算出
	float hitDistance = -b - sqrtf(D);
	if (hitDistance < 0.0f)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance < 0.0f)
		{
			return false;	// 交点が線分の外側にあり交差していない
		}
	}
	else if (hitDistance > a)
	{
		hitDistance = (-b + sqrtf(D));
		if (hitDistance > a)
		{
			return false;	// 交点が線分の外側にあり交差していない
		}
	}
	hitDistance /= a;

	// 解の公式の結果、円柱のstartCylinder側の底面の外で交差しているか確認
	if (md + hitDistance * nd < 0.0f)
	{
		// 底面との交差を確認し、交差していればhitDistanceを更新
		float t = -md / nd;
		if (mm + 2 * t * mn + t * t * nn <= radius * radius)
		{
			hitDistance = t;
		}
		else return false;
	}
	// 解の公式の結果、円柱のendCylinder側の上面の外で交差しているか確認
	else if (md + hitDistance * nd > dd)
	{
		// 上面との交差を確認し、交差していればhitDistanceを更新
		float t = -(md - dd) / nd;
		if (mm + 2 * t * mn - 2 * md + t * t * nn - 2 * t * nd + dd <= radius * radius)
		{
			hitDistance = t;
		}
		else return false;
	}

	// 線分が円柱の底面と上面の間で交差していることが確定（底面・上面での交差も含む）
	if (result)
	{
		// resultの各パラメータを算出する
		result->distance = rayDist * hitDistance;
		result->position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, result->distance));
		result->normal = DirectX::XMVectorNegate(rayDirection);

		if (onCenterLinPos)
		{
			float w = (md + nd * hitDistance) / dd;
			*onCenterLinPos = DirectX::XMVectorAdd(startCylinder, DirectX::XMVectorScale(d, w));
		}
	}

	return true;
}

// スフィアキャストVs三角形
bool Collision::IntersectSphereCastVsTriangle(
	const DirectX::XMVECTOR& sphereCastStart,
	const DirectX::XMVECTOR& sphereCastDirection,
	float sphereCastDist,
	float sphereCastRadius,
	const DirectX::XMVECTOR trianglePos[3],
	HitResult* result,
	bool firstSphereChk)
{
	// 三角形とスタート位置における球が交差している場合は、falseで終了する
	if (firstSphereChk)
	{
		if (IntersectSphereVsTriangle(sphereCastStart, sphereCastRadius, trianglePos))
		{
			return false;
		}
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR inverceDirection = DirectX::XMVectorScale(sphereCastDirection, -sphereCastDist);
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, inverceDirection));
	bool hitFlg = false;

	if (d >= 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		// 三角形の各頂点を法線を元に球半径だけ移動させる
		DirectX::XMVECTOR fixVec = DirectX::XMVectorScale(DirectX::XMVector3Normalize(norm), sphereCastRadius);

		// 移動後の三角形とスフィアキャストの中心レイが交差するなら、元の三角形の内部(面領域)でスフィアキャストが交差することが確定
		DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(sphereCastStart, DirectX::XMVectorAdd(trianglePos[0], fixVec));
		float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));

		if (t >= 0.0f && t < d)		// レイの向きと長さ確認
		{
			DirectX::XMVECTOR cross = DirectX::XMVector3Cross(inverceDirection, ap);
			float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, cross));
			if (v >= 0.0f && v <= d)
			{
				float w = -1.0f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, cross));
				if (w > 0.0f && v + w <= d)
				{
					if (result)
					{
						DirectX::XMVECTOR crossPos = DirectX::XMVectorAdd(sphereCastStart, DirectX::XMVectorScale(inverceDirection, -t / d));
						result->distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(crossPos, sphereCastStart)));
						DirectX::XMStoreFloat3(&result->position, DirectX::XMVectorSubtract(crossPos, fixVec));
						DirectX::XMStoreFloat3(&result->normal, DirectX::XMVector3Normalize(norm));
						DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
						DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
						DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
					}
					return true;
				}
			}
		}

		// 面領域で交差がなければ、ボロノイの各頂点領域、辺領域で交差判定を行い、最短距離を算出する
		enum class IntersectPattern
		{
			enNone = -1,
			enVertex0 = 1,
			enVertex1,
			enVertex2,
			enEdge01,
			enEdge02,
			enEdge12
		};
		HitResultVector tmpResult = {};
		DirectX::XMVECTOR minPosition = {}, minNormal = {}, tmpOnCenterLinePos = {}, minOnCenterLinePos = {};
		float minDistance = sphereCastDist;
		IntersectPattern minDistCalcPattern = IntersectPattern::enNone;

		// trianglePos[0] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex0;
				hitFlg = true;
			}
		}

		// trianglePos[1] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex1;
				hitFlg = true;
			}
		}

		// trianglePos[2] 頂点領域のチェック
		if (IntersectRayVsSphere(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[2], sphereCastRadius, &tmpResult))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minDistCalcPattern = IntersectPattern::enVertex2;
				hitFlg = true;
			}
		}


		// trianglePos[0]-trianglePos[1] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[1], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge01;
				hitFlg = true;
			}
		}

		// trianglePos[0]-trianglePos[2] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[0], trianglePos[2], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge02;
				hitFlg = true;
			}
		}

		// trianglePos[1]-trianglePos[2] 辺領域のチェック
		if (IntersectRayVsOrientedCylinder(sphereCastStart, sphereCastDirection, sphereCastDist, trianglePos[1], trianglePos[2], sphereCastRadius, &tmpResult, &tmpOnCenterLinePos))
		{
			if (minDistance > tmpResult.distance)
			{
				minDistance = tmpResult.distance;
				minPosition = tmpResult.position;
				minNormal = tmpResult.normal;
				minOnCenterLinePos = tmpOnCenterLinePos;
				minDistCalcPattern = IntersectPattern::enEdge12;
				hitFlg = true;
			}
		}


		// 交差が確定し、resultが有効ならHitResult情報を算出する
		if (hitFlg && result)
		{
			result->distance = minDistance;
			DirectX::XMStoreFloat3(&result->triangleVerts[0], trianglePos[0]);
			DirectX::XMStoreFloat3(&result->triangleVerts[1], trianglePos[1]);
			DirectX::XMStoreFloat3(&result->triangleVerts[2], trianglePos[2]);
			DirectX::XMStoreFloat3(&result->normal, DirectX::XMVector3Normalize(minNormal));

			// 交点(result->position)の算出は当たり方によって分岐
			switch (minDistCalcPattern)
			{
			case IntersectPattern::enVertex0:
				DirectX::XMStoreFloat3(&result->position, trianglePos[0]);
				break;
			case IntersectPattern::enVertex1:
				DirectX::XMStoreFloat3(&result->position, trianglePos[1]);
				break;
			case IntersectPattern::enVertex2:
				DirectX::XMStoreFloat3(&result->position, trianglePos[2]);
				break;
			case IntersectPattern::enEdge01:
			case IntersectPattern::enEdge02:
			case IntersectPattern::enEdge12:
				DirectX::XMStoreFloat3(&result->position, minOnCenterLinePos);
				break;
			default:
				return false;
			}
		}
	}

	return hitFlg;
}

// スフィアキャストVsモデル（メッシュ）
bool Collision::IntersectSphereCastVsModel(		// こちらはXMFLOAT3からXMVECTORへコンバート用の関数
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	float radius,
	const iModel* model,
	HitResultVector* result)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	HitResultVector resultVec = {};

	bool ret = Collision::IntersectSphereCastVsModel(WorldStart, WorldEnd, radius, model, &resultVec);

	if (result)
	{
		result->distance = resultVec.distance;
		result->position = resultVec.position;
		result->normal = resultVec.normal;
		result->materialIndex = resultVec.materialIndex;
		for (int i = 0; i < 3; ++i)
		{
			result->triangleVerts[i] = resultVec.triangleVerts[i];
		}
	}

	return ret;
}
bool Collision::IntersectSphereCastVsModel(
	const DirectX::XMVECTOR& start,
	const DirectX::XMVECTOR& end,
	float radius,
	const iModel* model,
	HitResultVector* result)
{
	DirectX::XMVECTOR WorldStart = start;
	DirectX::XMVECTOR WorldEnd = end;

	// ワールド空間のレイの長さ
	float minLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(WorldEnd, WorldStart)));

	HitResult tmpResult;

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュノード取得
		const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

		DirectX::XMVECTOR Start = {};	// スフィアキャストの始点
		DirectX::XMVECTOR End = {};		// スフィアキャストの終点

		bool calcLocal = true;	// ローカル空間で処理するかどうかのフラグ
		// 全軸等しいスケールでない場合、メッシュのローカル空間ではなく、ワールド空間で判定を行う
		if (node.scale.x != node.scale.y)		calcLocal = false;
		else if (node.scale.y != node.scale.z)	calcLocal = false;
		else if (node.scale.z != node.scale.x)	calcLocal = false;

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		if (calcLocal)	// ローカルの場合レイの始点・終点をワールド空間からローカル空間へ変換する
		{
			DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
			Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
			End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		}
		else	// ワールドで処理する場合はそのまま
		{
			Start = WorldStart;
			End = WorldEnd;
		}

		// レイの長さと正規化
		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(End, Start);
		float neart = DirectX::XMVectorGetX(DirectX::XMVector3Length(direction));
		direction = DirectX::XMVector3Normalize(direction);

		// 三角形（面）との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		DirectX::XMVECTOR HitVerts[3] = {};


		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR TrianglePos[3] = {
					DirectX::XMLoadFloat3(&a.position),
					DirectX::XMLoadFloat3(&b.position),
					DirectX::XMLoadFloat3(&c.position)
				};

				// スフィアキャストの実行前処理
				float scaledRadius = radius;
				if (calcLocal)	// ローカルの場合、半径をnode（三角形）のスケール倍する
				{
					scaledRadius *= node.scale.x;
				}
				else	// ワールドの場合、このタイミングで各頂点をワールド座標に変換しておく
				{
					TrianglePos[0] = DirectX::XMVector3TransformCoord(TrianglePos[0], WorldTransform);
					TrianglePos[1] = DirectX::XMVector3TransformCoord(TrianglePos[1], WorldTransform);
					TrianglePos[2] = DirectX::XMVector3TransformCoord(TrianglePos[2], WorldTransform);
				}

				// スフィアキャストVs三角形
				if (IntersectSphereCastVsTriangle(Start, direction, neart, scaledRadius, TrianglePos, &tmpResult))
				{
					// 最近距離を更新
					if (neart > tmpResult.distance)
					{
						HitPosition = DirectX::XMLoadFloat3(&tmpResult.position);
						HitNormal = DirectX::XMLoadFloat3(&tmpResult.normal);
						HitVerts[0] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[0]);
						HitVerts[1] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[1]);
						HitVerts[2] = DirectX::XMLoadFloat3(&tmpResult.triangleVerts[2]);
						neart = tmpResult.distance;
						materialIndex = subset.materialIndex;
					}
				}
			}
		}
		if (materialIndex >= 0)
		{
			// ヒット情報保存
			if (minLength > neart)
			{
				if (result)
				{
					if (calcLocal)	// ヒット情報をローカル空間からワールド空間へ変換
					{
						result->position = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
						result->normal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
						for (int i = 0; i < 3; ++i)
						{
							DirectX::XMVECTOR Vertex = DirectX::XMVector3Transform(HitVerts[i], WorldTransform);
							result->triangleVerts[i] = Vertex;
						}
					}
					else
					{
						result->position = HitPosition;
						result->normal = HitNormal;
						for (int i = 0; i < 3; ++i)
						{
							result->triangleVerts[i] = HitVerts[i];
						}
					}
					result->normal = DirectX::XMVector3Normalize(result->normal);
					result->distance = neart;
					result->materialIndex = materialIndex;
				}

				minLength = neart;
				hit = true;
			}
		}
	}

	return hit;
}

// AABBと平面の最短距離を取得する	※返り値がマイナスだと交差している状況と定義
float Collision::GetMinDist_AABBPlane(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const DirectX::XMVECTOR& planeNorm,	// 要正規化
	const float planeDist,
	bool* reverse)		// 法線に対して裏か表かを判断するようのフラグ
{
	// planeNormの各軸の絶対値を取って内積
	DirectX::XMVECTOR normAbs = DirectX::XMVectorAbs(planeNorm);
	float projRadii = DirectX::XMVectorGetX(DirectX::XMVector3Dot(aabbRadii, normAbs));	//常に正
	float dist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(aabbPos, planeNorm)) - planeDist;	//裏なら負

	if (reverse)
	{
		if (dist < 0.0f)
		{
			*reverse = true;
		}
		else
		{
			*reverse = false;
		}
	}

	float ret = fabsf(dist) - projRadii;
	return ret;
}


// AABBvs三角形の交差判定
bool Collision::IntersectAABBVsTriangle(
	const DirectX::XMVECTOR& aabbPos,
	const DirectX::XMVECTOR& aabbRadii,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result)
{
	// 計算の高速化のため、AABBを原点に移動して判定を行う
	// そのため三角形を合わせて移動させる。合わせて成分計算がメインになるためFLOAT3にしておく。
	DirectX::XMFLOAT3 movedTrianglePos[3] = {};
	DirectX::XMFLOAT3 triangleEdge[3] = {};

	DirectX::XMStoreFloat3(&movedTrianglePos[0], DirectX::XMVectorSubtract(trianglePos[0], aabbPos));
	DirectX::XMStoreFloat3(&movedTrianglePos[1], DirectX::XMVectorSubtract(trianglePos[1], aabbPos));
	DirectX::XMStoreFloat3(&movedTrianglePos[2], DirectX::XMVectorSubtract(trianglePos[2], aabbPos));

	DirectX::XMStoreFloat3(&triangleEdge[0], DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]));
	DirectX::XMStoreFloat3(&triangleEdge[1], DirectX::XMVectorSubtract(trianglePos[2], trianglePos[1]));
	DirectX::XMStoreFloat3(&triangleEdge[2], DirectX::XMVectorSubtract(trianglePos[0], trianglePos[2]));

	// 外積分離軸の判定
	// // 計算用変数
	float proj1 = 0.0f;				// 三角形の射影長１　（外積分離軸と直交する辺を構成する頂点）
	float proj2 = 0.0f;				// 三角形の射影長２　（外積分離軸と直交する辺を構成しない頂点）
	float r = 0.0f;					// AABBの射影長
	float penetration = 0.0f;		// 計算中のめり込み量
	float minPenetration = FLT_MAX;	// 最小めり込み量
	int minType = -1;				// 最小交差状態Ｎｏ
	DirectX::XMVECTOR norm = {}, minNorm = {};
	float axisLength = 0.0f;		// 外積分離軸長

	// xNorm × Edge_N
	for (int i = 0; i < 3; i++)
	{
		norm = { 0, -triangleEdge[i].z, triangleEdge[i].y };

		axisLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(norm));
		if (axisLength == 0) continue;	// 面法線と辺が平行ならcontinue

		r = DirectX::XMVectorGetY(aabbRadii) * fabs(triangleEdge[i].z) + DirectX::XMVectorGetZ(aabbRadii) * fabs(triangleEdge[i].y);
		proj1 = -movedTrianglePos[i].y * movedTrianglePos[(i + 1) % 3].z + movedTrianglePos[i].z * movedTrianglePos[(i + 1) % 3].y;
		proj2 = -movedTrianglePos[(i + 2) % 3].y * triangleEdge[i].z + movedTrianglePos[(i + 2) % 3].z * triangleEdge[i].y;

		penetration = r - std::max(-std::max(proj1, proj2), std::min(proj1, proj2));
		if (penetration <= 0.0f) return false;
		if (result && penetration < minPenetration)
		{
			minPenetration = penetration;
			minNorm = norm;
			minType = i;
		}
	}

	// yNorm × Edge_N
	for (int i = 0; i < 3; i++)
	{
		norm = { triangleEdge[i].z, 0, -triangleEdge[i].x };

		axisLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(norm));
		if (axisLength == 0) continue;	// 面法線と辺が平行ならcontinue

		r = DirectX::XMVectorGetX(aabbRadii) * fabs(triangleEdge[i].z) + DirectX::XMVectorGetZ(aabbRadii) * fabs(triangleEdge[i].x);
		proj1 = -movedTrianglePos[i].z * movedTrianglePos[(i + 1) % 3].x + movedTrianglePos[i].x * movedTrianglePos[(i + 1) % 3].z;
		proj2 = -movedTrianglePos[(i + 2) % 3].z * triangleEdge[i].x + movedTrianglePos[(i + 2) % 3].x * triangleEdge[i].z;

		penetration = r - std::max(-std::max(proj1, proj2), std::min(proj1, proj2));
		if (penetration <= 0.0f) return false;
		if (result && penetration < minPenetration)
		{
			minPenetration = penetration;
			minNorm = norm;
			minType = i + 3;
		}
	}

	// zNorm × Edge_N
	for (int i = 0; i < 3; i++)
	{
		norm = { -triangleEdge[i].y, triangleEdge[i].x, 0 };

		axisLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(norm));
		if (axisLength == 0) continue;	// 面法線と辺が平行ならcontinue

		r = DirectX::XMVectorGetX(aabbRadii) * fabs(triangleEdge[i].y) + DirectX::XMVectorGetY(aabbRadii) * fabs(triangleEdge[i].x);
		proj1 = -movedTrianglePos[i].x * movedTrianglePos[(i + 1) % 3].y + movedTrianglePos[i].y * movedTrianglePos[(i + 1) % 3].x;
		proj2 = -movedTrianglePos[(i + 2) % 3].x * triangleEdge[i].y + movedTrianglePos[(i + 2) % 3].y * triangleEdge[i].x;

		penetration = r - std::max(-std::max(proj1, proj2), std::min(proj1, proj2));
		if (penetration <= 0.0f) return false;
		if (result && penetration < minPenetration)
		{
			minPenetration = penetration;
			minNorm = norm;
			minType = i + 6;
		}
	}

	// AABBの面法線軸の判定
	// xNorm+
	penetration = DirectX::XMVectorGetX(aabbRadii) - std::min(std::min(movedTrianglePos[0].x, movedTrianglePos[1].x), movedTrianglePos[2].x);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 10;
	}

	// xNorm-
	penetration = DirectX::XMVectorGetX(aabbRadii) + std::max(std::max(movedTrianglePos[0].x, movedTrianglePos[1].x), movedTrianglePos[2].x);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 9;
	}

	// yNorm+
	penetration = DirectX::XMVectorGetY(aabbRadii) - std::min(std::min(movedTrianglePos[0].y, movedTrianglePos[1].y), movedTrianglePos[2].y);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 12;
	}

	// yNorm-
	penetration = DirectX::XMVectorGetY(aabbRadii) + std::max(std::max(movedTrianglePos[0].y, movedTrianglePos[1].y), movedTrianglePos[2].y);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 11;
	}

	// zNorm+
	penetration = DirectX::XMVectorGetZ(aabbRadii) - std::min(std::min(movedTrianglePos[0].z, movedTrianglePos[1].z), movedTrianglePos[2].z);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 14;
	}

	// zNorm-
	penetration = DirectX::XMVectorGetZ(aabbRadii) + std::max(std::max(movedTrianglePos[0].z, movedTrianglePos[1].z), movedTrianglePos[2].z);
	if (penetration <= 0.0f) return false;
	if (result && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 13;
	}


	// 三角形の面法線軸の判定
	DirectX::XMVECTOR planeNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&triangleEdge[0]), DirectX::XMLoadFloat3(&triangleEdge[1])));
	bool reverseChk = true;
	float planeDist = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&movedTrianglePos[0]), planeNorm));
	penetration = -GetMinDist_AABBPlane({}, aabbRadii, planeNorm, planeDist, &reverseChk);
	if (penetration < 0.0f) return false;	// 他の計算のpenetrationは触れている（==0.0f）でもOKだが、最後のここは触れているだけならめり込んでいない判定にする
	if (result && penetration > 0.0f && penetration < minPenetration)
	{
		minPenetration = penetration;
		minType = 15;
	}

	// 法線算出
	if (result)
	{
		switch (minType)
		{
		case -1: return false;
		case 9:
			result->normal = { 1.0f,0.0f,0.0f }; break;
		case 10:
			result->normal = { -1.0f,0.0f,0.0f }; break;
		case 11:
			result->normal = { 0.0f,1.0f,0.0f }; break;
		case 12:
			result->normal = { 0.0f,-1.0f,0.0f }; break;
		case 13:
			result->normal = { 0.0f,0.0f,1.0f }; break;
		case 14:
			result->normal = { 0.0f,0.0f,-1.0f }; break;
		case 15:
			result->normal = reverseChk ? DirectX::XMVectorNegate(planeNorm) : planeNorm;
			break;
		default:
			result->normal = DirectX::XMVector3Normalize(minNorm);

			// AABBと三角形の中心間ベクトルから法線の方向を補正
			if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(result->normal, DirectX::XMLoadFloat3(&movedTrianglePos[0]))) > 0.0f)
			{
				result->normal = DirectX::XMVectorNegate(result->normal);
			}

			break;
		}

		result->penetration = minPenetration;
	}

	return true;
}

// AABBvsAABBの交差判定
bool Collision::IntersectAABBVsAABB(
	const DirectX::XMVECTOR& aabbPos1,
	const DirectX::XMVECTOR& aabbRadii1,
	const DirectX::XMVECTOR& aabbPos2,
	const DirectX::XMVECTOR& aabbRadii2,
	IntersectionResult* result)
{
	float dx = DirectX::XMVectorGetX(aabbRadii1) + DirectX::XMVectorGetX(aabbRadii2) - fabsf(DirectX::XMVectorGetX(aabbPos1) - DirectX::XMVectorGetX(aabbPos2));
	if (dx < 0.0f)
	{
		return false;
	}

	float dy = DirectX::XMVectorGetY(aabbRadii1) + DirectX::XMVectorGetY(aabbRadii2) - fabsf(DirectX::XMVectorGetY(aabbPos1) - DirectX::XMVectorGetY(aabbPos2));
	if (dy < 0.0f)
	{
		return false;
	}

	float dz = DirectX::XMVectorGetZ(aabbRadii1) + DirectX::XMVectorGetZ(aabbRadii2) - fabsf(DirectX::XMVectorGetZ(aabbPos1) - DirectX::XMVectorGetZ(aabbPos2));
	if (dz < 0.0f)
	{
		return false;
	}

	if (result)
	{
		float minAxis = std::min(std::min(dx, dy), dz);
		if (minAxis == dx)
		{
			result->normal = (DirectX::XMVectorGetX(aabbPos1) > DirectX::XMVectorGetX(aabbPos2)) ? DirectX::XMVECTOR{ 1.0f, 0.0f, 0.0f } : DirectX::XMVECTOR{ -1.0f, 0.0f, 0.0f };
			result->penetration = dx;
		}
		else if (minAxis == dy)
		{
			result->normal = (DirectX::XMVectorGetY(aabbPos1) > DirectX::XMVectorGetY(aabbPos2)) ? DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f } : DirectX::XMVECTOR{ 0.0f, -1.0f, 0.0f };
			result->penetration = dy;
		}
		else if (minAxis == dz)
		{
			result->normal = (DirectX::XMVectorGetZ(aabbPos1) > DirectX::XMVectorGetZ(aabbPos2)) ? DirectX::XMVECTOR{ 0.0f, 0.0f, 1.0f } : DirectX::XMVECTOR{ 0.0f, 0.0f, -1.0f };
			result->penetration = dz;
		}

		result->pointA = DirectX::XMVectorSubtract(aabbPos1, DirectX::XMVectorMultiply(aabbRadii1, result->normal));
		result->pointB = DirectX::XMVectorAdd(aabbPos2, DirectX::XMVectorMultiply(aabbRadii2, result->normal));
	}

	return true;
}

// 線分と線分の最短距離の二乗を取得する
float Collision::GetMinDistSq_SegmentSegment(
	const  DirectX::XMVECTOR& point1A,
	const  DirectX::XMVECTOR& point1B,
	const  DirectX::XMVECTOR& point2A,
	const  DirectX::XMVECTOR& point2B,
	DirectX::XMVECTOR* nearPoint1,
	DirectX::XMVECTOR* nearPoint2)
{
	DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
	DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
	DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

	float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
	float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
	float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

	float t1 = 0.0f, t2 = 0.0f;

	if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
	{
		t1 = t2 = 0.0f;
	}
	else if (a <= FLT_EPSILON)					// 片方（d0）が点に縮退している場合
	{
		t1 = 0.0f;
		t2 = std::clamp(f / e, 0.0f, 1.0f);
	}
	else
	{
		float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
		if (e <= FLT_EPSILON)					// 片方（d1）が点に縮退している場合
		{
			t2 = 0.0f;
			t1 = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else									// 両方が線分
		{
			float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
			float denom = a * e - b * b;

			if (denom != 0.0f)					// 平行確認（平行時は t1 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
			{
				t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}

			t2 = b * t1 + f;

			if (t2 < 0.0f)						// t1が始端より外側にある場合
			{
				t2 = 0.0f;
				t1 = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t2 > e)					// t1が終端より外側にある場合
			{
				t2 = 1.0f;
				t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
			else								// t1が線分上にある場合
			{
				t2 /= e;
			}
		}
	}

	// 各線分上の最近点算出
	DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
	DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

	if (nearPoint1)
	{
		*nearPoint1 = point1;
	}
	if (nearPoint2)
	{
		*nearPoint2 = point2;
	}

	return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

// 線分と三角形の最短距離の二乗を取得する
float Collision::GetMinDistSq_SegmentTriangle(
	const  DirectX::XMVECTOR& pointA,			// 線分端A
	const  DirectX::XMVECTOR& pointB,			// 線分端B
	const  DirectX::XMVECTOR trianglePos[3],	// 三角形頂点
	DirectX::XMVECTOR* nearPointSegment,		// 線分側の最近点
	DirectX::XMVECTOR* nearPointTriangle,		// 三角形側の最近点
	bool& nearPointEdghFlg)						// 辺（頂点）が最近点の場合true
{
	// いったん面が最近点としておく
	nearPointEdghFlg = false;

	// 線分が三角形と交差するか確認
	DirectX::XMVECTOR rayDirection = DirectX::XMVectorSubtract(pointB, pointA);
	float rayDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(rayDirection));
	HitResultVector result;
	if (IntersectRayVsTriangle(pointA, rayDirection, trianglePos, result))
	{
		*nearPointSegment = result.position;
		*nearPointTriangle = result.position;
		return 0;
	}
	// 逆向き
	rayDirection = DirectX::XMVectorSubtract(pointA, pointB);
	if (IntersectRayVsTriangle(pointB, rayDirection, trianglePos, result))
	{
		*nearPointSegment = result.position;
		*nearPointTriangle = result.position;
		return 0;
	}

	// 線分の各頂点に対し、三角形の最近点を求め、あわせて三角形の内部にあるか外部にあるか確認
	DirectX::XMVECTOR nearPosA = {};
	bool insideA = GetClosestPoint_PointTriangle(pointA, trianglePos, nearPosA);
	DirectX::XMVECTOR nearPosB = {};
	bool insideB = GetClosestPoint_PointTriangle(pointB, trianglePos, nearPosB);


	// いずれかの点を射影した点が三角形内部に収まる場合、その点が最短点になる可能性を考え、距離を記録する
	// また、両方の点が三角形を含む平面を境界に同じ側に存在し、射影した場合に三角形内部に収まる場合は
	// 交差していないため、線分のどちらかの端点が最短箇所になり処理終了
	float nearDistSq = FLT_MAX;
	if (insideA || insideB)
	{
		float distSqA = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(nearPosA, pointA)));
		float distSqB = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(nearPosB, pointB)));

		if (distSqA < distSqB)
		{
			nearDistSq = distSqA;
			*nearPointSegment = pointA;
			*nearPointTriangle = nearPosA;
		}
		else
		{
			nearDistSq = distSqB;
			*nearPointSegment = pointB;
			*nearPointTriangle = nearPosB;
		}

		if (insideA && insideB)	// 両端とも射影点が三角形内部
		{
			return nearDistSq;
		}
	}

	// ここまで来た段階で、辺（頂点を含む）での交差が確定するため、nearPointEdghFlgをtrueにしておく
	// (カプセルと三角形の交差処理の法線の算出に面との交差なのか、辺（頂点）との交差なのかの情報が必要なため)
	nearPointEdghFlg = true;

	// 三角形外部に線分（または端）が存在し、最近点が辺上になる場合、線分と三角形の３辺の距離を全て計測し比較する
	DirectX::XMVECTOR pointSegment = {};
	DirectX::XMVECTOR pointEdge = {};

	float distSqEdge = GetMinDistSq_SegmentSegment(pointA, pointB, trianglePos[0], trianglePos[1], &pointSegment, &pointEdge);
	if (distSqEdge < nearDistSq)
	{
		nearDistSq = distSqEdge;
		*nearPointSegment = pointSegment;
		*nearPointTriangle = pointEdge;
	}

	distSqEdge = GetMinDistSq_SegmentSegment(pointA, pointB, trianglePos[1], trianglePos[2], &pointSegment, &pointEdge);
	if (distSqEdge < nearDistSq)
	{
		nearDistSq = distSqEdge;
		*nearPointSegment = pointSegment;
		*nearPointTriangle = pointEdge;
	}

	distSqEdge = GetMinDistSq_SegmentSegment(pointA, pointB, trianglePos[2], trianglePos[0], &pointSegment, &pointEdge);
	if (distSqEdge < nearDistSq)
	{
		nearDistSq = distSqEdge;
		*nearPointSegment = pointSegment;
		*nearPointTriangle = pointEdge;
	}

	return nearDistSq;
}

// カプセルvs三角形の交差判定
bool Collision::IntersectCapsuleVsTriangle(
	const DirectX::XMVECTOR& position,
	const DirectX::XMVECTOR& direction,
	float radius,
	float length,
	const DirectX::XMVECTOR trianglePos[3],
	IntersectionResult* result)
{
	DirectX::XMVECTOR nearPointSegment = {};
	DirectX::XMVECTOR nearPointTriangle = {};

	bool ret = false;
	DirectX::XMVECTOR end = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, length));
	bool nearPointEdghFlg;

	// 線分と三角形の距離を計算する
	float distSq = GetMinDistSq_SegmentTriangle(position, end, trianglePos, &nearPointSegment, &nearPointTriangle, nearPointEdghFlg);
	if (distSq < radius * radius)
	{
		ret = true;
	}

	// 衝突処理や押し戻し処理用にresultにパラメータを設定する
	if (result)
	{
		// pointA,pointB設定
		result->pointA = nearPointSegment;
		result->pointB = nearPointTriangle;

		if (ret)
		{
			// normal, penetration設定
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(result->pointA, result->pointB);
			if (!nearPointEdghFlg)
			{
				// 最短点が面上の場合
				DirectX::XMVECTOR triangleNorm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
					DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]), DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0])));

				if (distSq == 0)	// 線分が三角形を貫通している場合
				{
					// 両端の表裏判定
					float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(triangleNorm, direction));
					DirectX::XMVECTOR backPoint = d > 0 ? position : end;

					// 裏側の点から交点へのベクトルを算出し、カプセルのポジションに加算
					// さらに法線の単位ベクトルにカプセルの半径を掛けたベクトルを用意し、カプセルのポジションに加算する
					vec = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(nearPointTriangle, backPoint), DirectX::XMVectorScale(triangleNorm, radius));

					result->penetration = DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
					result->normal = DirectX::XMVector3Normalize(vec);
				}
				else
				{
					result->penetration = radius - DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
					result->normal = DirectX::XMVector3Normalize(vec);
				}
			}
			else
			{
				// 最短点が辺上の場合
				result->penetration = radius - DirectX::XMVectorGetX(DirectX::XMVector3Length(vec));
				result->normal = DirectX::XMVector3Normalize(vec);
			}
		}
	}

	return ret;
}

// カプセルvsカプセルの交差判定
bool Collision::IntersectCapsuleVsCapsule(
	const DirectX::XMVECTOR& position1,	// 中心
	const DirectX::XMVECTOR& direction1,	// 向き（正規化）
	const float					length1,	// 長さ
	const float					radius1,	// 半径
	const DirectX::XMVECTOR& position2,	// 中心
	const DirectX::XMVECTOR& direction2,	// 向き（正規化）
	const float					length2,	// 長さ
	const float					radius2,	// 半径
	IntersectionResult* result)
{
	// 各カプセルの中心線上の最近点算出
	DirectX::XMVECTOR point1 = {}, point2 = {};
	DirectX::XMVECTOR end1 = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(direction1, length1));
	DirectX::XMVECTOR end2 = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(direction2, length2));

	float distSq = GetMinDistSq_SegmentSegment(position1, end1, position2, end2, &point1, &point2);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
	float radiusAdd = radius1 + radius2;

	if (distSq < radiusAdd * radiusAdd)
	{
		if (result)
		{
			result->normal = DirectX::XMVector3Normalize(vec);
			result->penetration = radiusAdd - sqrtf(distSq);
			result->pointA = DirectX::XMVectorAdd(point1, DirectX::XMVectorScale(result->normal, -radius1));
			result->pointB = DirectX::XMVectorAdd(point2, DirectX::XMVectorScale(result->normal, radius2));
		}

		return true;
	}

	return false;
}