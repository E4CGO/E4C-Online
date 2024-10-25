#include "TAKOEngine/Physics/MapCollider.h"

#include "TAKOEngine/Tool/XMFLOAT.h"

MapCollider::MapCollider(iModel* model)
{
	this->model = model;

	type = COLLIDER_TYPE::MAP;

	//// 8分木作成
	//XMFLOAT3 minPos, maxPos;
	//CalcMapArea(minPos, maxPos);
	//tree.Initialize(7,
	//	minPos.x - 1.0f, maxPos.x + 1.0f,
	//	minPos.y - 50.0f, maxPos.y + 10.0f,
	//	minPos.z - 1.0f, maxPos.z + 1.0f);// エリアを少し大きめに作成
	//RegisterPorigons();
}

MapCollider::~MapCollider()
{
	//tree.Finalize();
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
		// メッシュノード取得
		const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// 球体をワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR SphereCenter = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenter), InverseWorldTransform);
		DirectX::XMVECTOR SphereCenterOri = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&sphereCenterOri), InverseWorldTransform);
		float sphereRadius = other->GetScale().x; // 球体半径
		DirectX::XMVECTOR SphereRadius = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat(&sphereRadius), InverseWorldTransform);
		DirectX::XMStoreFloat(&sphereRadius, SphereRadius);

		// 三角形（面）との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;

		for (UINT i = 0; i < indices.size(); i += 3)
		{
			// 三角形の頂点を抽出
			//const ModelResource::Vertex& a = vertices.at(indices.at(i));
			//const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
			//const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));
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

bool MapCollider::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& result
)
{
	bool hit = false;
	HitResultVector ret;
	hit = Collision::IntersectRayVsModel(start, end, model, ret);
	XMStoreFloat3(&result.position, ret.position);
	XMStoreFloat3(&result.normal, ret.normal);
	result.distance = ret.distance;
	result.materialIndex = ret.materialIndex;
	XMStoreFloat3(&result.triangleVerts[0], ret.triangleVerts[0]);
	XMStoreFloat3(&result.triangleVerts[1], ret.triangleVerts[1]);
	XMStoreFloat3(&result.triangleVerts[2], ret.triangleVerts[2]);
	return hit;
	//// 最小値最大値
	//float minX, maxX;
	//if (start.x < end.x)
	//{
	//	minX = start.x;
	//	maxX = end.x;
	//}
	//else
	//{
	//	minX = end.x;
	//	maxX = start.x;
	//}
	//
	//float minY, maxY;
	//if (start.y < end.y)
	//{
	//	minY = start.y;
	//	maxY = end.y;
	//}
	//else
	//{
	//	minY = end.y;
	//	maxY = start.y;
	//}

	//float minZ, maxZ;
	//if (start.z < end.z)
	//{
	//	minZ = start.z;
	//	maxZ = end.z;
	//}
	//else
	//{
	//	minZ = end.z;
	//	maxZ = start.z;
	//}

	//// レイが通る空間の配列番号算出
	//int Elem = tree.GetLinerIndex(minX, maxX, minY, maxY, minZ, maxZ);

	//bool hit = false;
	//SearchChildren(Elem, start, end, result, hit);
	//SearchParent(Elem, start, end, result, hit);
	//return hit;
}

//void MapCollider::CalcMapArea(
//	DirectX::XMFLOAT3& minPos,
//	DirectX::XMFLOAT3& maxPos
//)
//{
//	minPos = { FLT_MAX, FLT_MAX , FLT_MAX };
//	maxPos = { FLT_MIN, FLT_MIN , FLT_MIN };
//
//	const ModelResource* resource = model->GetResource();
//	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
//	{
//		// 頂点データ取得
//		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
//
//		XMFLOAT3 tmp_minPos = vertices.at(0).position;
//		XMFLOAT3 tmp_maxPos = vertices.at(0).position;
//
//		for (UINT i = 1; i < vertices.size(); i++)
//		{
//			if (vertices.at(i).position.x < tmp_minPos.x)
//			{
//				tmp_minPos.x = vertices.at(i).position.x;
//			}
//			else if (vertices.at(i).position.x > tmp_maxPos.x)
//			{
//				tmp_maxPos.x = vertices.at(i).position.x;
//			}
//
//			if (vertices.at(i).position.y < tmp_minPos.y)
//			{
//				tmp_minPos.y = vertices.at(i).position.y;
//			}
//			else if (vertices.at(i).position.y > tmp_maxPos.y)
//			{
//				tmp_maxPos.y = vertices.at(i).position.y;
//			}
//
//			if (vertices.at(i).position.z < tmp_minPos.z)
//			{
//				tmp_minPos.z = vertices.at(i).position.z;
//			}
//			else if (vertices.at(i).position.z > tmp_maxPos.z)
//			{
//				tmp_maxPos.z = vertices.at(i).position.z;
//			}
//		}
//
//		// メッシュノード取得
//		const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);
//		// ワールド行列
//		XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);
//
//		// 頂点をローカル空間からワールド空間へ変換
//		XMVECTOR MinP = XMLoadFloat3(&tmp_minPos);
//		XMVECTOR MaxP = XMLoadFloat3(&tmp_maxPos);
//		MinP = XMVector3TransformCoord(MinP, WorldTransform);
//		MaxP = XMVector3TransformCoord(MaxP, WorldTransform);
//		XMStoreFloat3(&tmp_minPos, MinP);
//		XMStoreFloat3(&tmp_maxPos, MaxP);
//		// ここで値の大小が逆になってる可能性があるのでもう一度比較
//		if (tmp_minPos.x > tmp_maxPos.x)
//		{
//			float tmp = tmp_maxPos.x;
//			tmp_maxPos.x = tmp_minPos.x;
//			tmp_minPos.x = tmp;
//		}
//		if (tmp_minPos.y > tmp_maxPos.y)
//		{
//			float tmp = tmp_maxPos.y;
//			tmp_maxPos.y = tmp_minPos.y;
//			tmp_minPos.y = tmp;
//		}
//		if (tmp_minPos.z > tmp_maxPos.z)
//		{
//			float tmp = tmp_maxPos.z;
//			tmp_maxPos.z = tmp_minPos.z;
//			tmp_minPos.z = tmp;
//		}
//
//		// これまでに見つけたものと比較
//		if (minPos.x > tmp_minPos.x)
//		{
//			minPos.x = tmp_minPos.x;
//		}
//		if (maxPos.x < tmp_maxPos.x)
//		{
//			maxPos.x = tmp_maxPos.x;
//		}
//
//		if (minPos.y > tmp_minPos.y)
//		{
//			minPos.y = tmp_minPos.y;
//		}
//		if (maxPos.y < tmp_maxPos.y)
//		{
//			maxPos.y = tmp_maxPos.y;
//		}
//
//		if (minPos.z > tmp_minPos.z)
//		{
//			minPos.z = tmp_minPos.z;
//		}
//		if (maxPos.z < tmp_maxPos.z)
//		{
//			maxPos.z = tmp_maxPos.z;
//		}
//	}
//}

//void MapCollider::RegisterPorigons()
//{
//	const ModelResource* resource = model->GetResource();
//	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
//	{
//		// メッシュノード取得
//		const iModel::Node& node = model->GetNodes().at(mesh.nodeIndex);
//		
//		// ワールド行列取得
//		XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
//
//		// 頂点データ取得
//		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
//		const std::vector<UINT> indices = mesh.indices;
//
//		for (UINT i = 0; i < indices.size(); i += 3)//102837
//		{
//			// 三角形の頂点を抽出
//			//const ModelResource::Vertex& a = vertices.at(indices.at(i));
//			//const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
//			//const ModelResource::Vertex& c = vertices.at(indices.at(i + 2));
//			const ModelResource::Vertex& a = vertices.at(indices.at(i + 2));
//			const ModelResource::Vertex& b = vertices.at(indices.at(i + 1));
//			const ModelResource::Vertex& c = vertices.at(indices.at(i));
//
//			XMVECTOR A = XMLoadFloat3(&a.position);
//			XMVECTOR B = XMLoadFloat3(&b.position);
//			XMVECTOR C = XMLoadFloat3(&c.position);
//
//			// 三角形の三辺ベクトルを算出
//			XMVECTOR AB = XMVectorSubtract(B, A);
//			XMVECTOR BC = XMVectorSubtract(C, B);
//			XMVECTOR CA = XMVectorSubtract(A, C);
//
//			// 三角形の法線ベクトルを算出
//			XMVECTOR N = XMVector3Cross(AB, BC);
//
//			// 頂点と法線をローカル空間からワールド空間へ変換
//			A = XMVector3TransformCoord(A, WorldTransform);
//			B = XMVector3TransformCoord(B, WorldTransform);
//			C = XMVector3TransformCoord(C, WorldTransform);
//			N = XMVector3TransformNormal(N, WorldTransform);
//			N = XMVector3Normalize(N);
//
//			// Porigon構造体に格納
//			Porigon* porigon = new Porigon;
//			XMStoreFloat3(&porigon->position[0], A);
//
//			//// 頂点の順番を補正
//			XMVECTOR WorldAB = XMVectorSubtract(B, A);
//			XMVECTOR WorldAC = XMVectorSubtract(C, A);
//			if (XMVectorGetX(XMVector3Dot(N, XMVector3Cross(WorldAB, WorldAC))) > 0)
//			{
//				XMStoreFloat3(&porigon->position[1], B);
//				XMStoreFloat3(&porigon->position[2], C);
//			}
//			else
//			{
//				XMStoreFloat3(&porigon->position[1], C);
//				XMStoreFloat3(&porigon->position[2], B);
//			}
//			
//			XMStoreFloat3(&porigon->normal, N);
//			porigon->materialIndex = mesh.materialIndex;
//
//
//			/////////// 8分木に登録 ////////////
//			// 頂点位置の最小値最大値
//			float minX, maxX;
//			if (porigon->position[0].x < porigon->position[1].x)
//			{
//				if (porigon->position[0].x < porigon->position[2].x)
//				{
//					minX = porigon->position[0].x;
//					maxX = porigon->position[1].x < porigon->position[2].x ?
//						porigon->position[2].x : porigon->position[1].x;
//				}
//				else
//				{
//					minX = porigon->position[2].x;
//					maxX = porigon->position[1].x;
//				}
//			}
//			else
//			{
//				if (porigon->position[1].x < porigon->position[2].x)
//				{
//					minX = porigon->position[1].x;
//					maxX = porigon->position[0].x < porigon->position[2].x ?
//						porigon->position[2].x : porigon->position[0].x;
//				}
//				else
//				{
//					minX = porigon->position[2].x;
//					maxX = porigon->position[0].x;
//				}
//			}
//
//			float minY, maxY;
//			if (porigon->position[0].y < porigon->position[1].y)
//			{
//				if (porigon->position[0].y < porigon->position[2].y)
//				{
//					minY = porigon->position[0].y;
//					maxY = porigon->position[1].y < porigon->position[2].y ?
//						porigon->position[2].y : porigon->position[1].y;
//				}
//				else
//				{
//					minY = porigon->position[2].y;
//					maxY = porigon->position[1].y;
//				}
//			}
//			else
//			{
//				if (porigon->position[1].y < porigon->position[2].y)
//				{
//					minY = porigon->position[1].y;
//					maxY = porigon->position[0].y < porigon->position[2].y ?
//						porigon->position[2].y : porigon->position[0].y;
//				}
//				else
//				{
//					minY = porigon->position[2].y;
//					maxY = porigon->position[0].y;
//				}
//			}
//
//			float minZ, maxZ;
//			if (porigon->position[0].z < porigon->position[1].z)
//			{
//				if (porigon->position[0].z < porigon->position[2].z)
//				{
//					minZ = porigon->position[0].z;
//					maxZ = porigon->position[1].z < porigon->position[2].z ?
//						porigon->position[2].z : porigon->position[1].z;
//				}
//				else
//				{
//					minZ = porigon->position[2].z;
//					maxZ = porigon->position[1].z;
//				}
//			}
//			else
//			{
//				if (porigon->position[1].z < porigon->position[2].z)
//				{
//					minZ = porigon->position[1].z;
//					maxZ = porigon->position[0].z < porigon->position[2].z ?
//						porigon->position[2].z : porigon->position[0].z;
//				}
//				else
//				{
//					minZ = porigon->position[2].z;
//					maxZ = porigon->position[0].z;
//				}
//			}
//
//			// 登録
//			tree.Regist(minX, maxX, minY, maxY, minZ, maxZ, porigon);
//		}
//	}
//}

//bool MapCollider::ShpereVsPorigon(
//	Collider*& other,
//	DirectX::XMFLOAT3& direction,
//	Porigon* porigon,
//	HitResult& result
//)
//{
//	DirectX::XMVECTOR Vertex[3] =
//	{
//		XMLoadFloat3(&porigon->position[0]),
//		XMLoadFloat3(&porigon->position[1]),
//		XMLoadFloat3(&porigon->position[2])
//	};
//
//	DirectX::XMVECTOR Edge[3] =
//	{
//		XMVectorSubtract(Vertex[1], Vertex[0]),
//		XMVectorSubtract(Vertex[2], Vertex[1]),
//		XMVectorSubtract(Vertex[0], Vertex[2])
//	};
//
//	// 面法線
//	XMVECTOR Normal = XMLoadFloat3(&porigon->normal);
//
//	// 平面と移動前後の球の距離を求める
//	XMFLOAT3 sphereCenter = other->GetPosition();
//	XMFLOAT3 sphereCenterOri = other->GetPosition() + direction;
//	XMVECTOR  SphereCenter = XMLoadFloat3(&sphereCenter);
//	XMVECTOR  SphereCenterOri =  XMLoadFloat3(&sphereCenterOri);
//	XMVECTOR  Dot1 = XMVector3Dot(Normal, SphereCenter);
//	XMVECTOR  Dot2 = XMVector3Dot(Normal, SphereCenterOri);
//	XMVECTOR  Dot3 = XMVector3Dot(Normal, Vertex[0]);
//	XMVECTOR  Distance = XMVectorSubtract(Dot1, Dot3);
//	XMVECTOR  DistanceOri = XMVectorSubtract(Dot2, Dot3);
//	float distance = XMVectorGetX(Distance);
//	float distanceOri = XMVectorGetX(DistanceOri);
//
//	// 移動前の球が面の裏側なら当たらない
//	if (distance < 0)	return false;
//
//	// 移動後の球も面の表側でその距離が半径以上なら当たらない
//	float sphereRadius = other->GetScale().x; // 球体半径
//	if (distanceOri > sphereRadius)	return false;
//
//	// 球が三角形内部にあるか
//	bool outside = false;
//	XMVECTOR Vec[3];
//	for (int i = 0; i < 3; ++i)
//	{
//		Vec[i] = XMVectorSubtract(SphereCenter, Vertex[i]);
//		XMVECTOR Cross = XMVector3Cross(Edge[i], Vec[i]);
//		XMVECTOR Dot = XMVector3Dot(Normal, Cross);
//		if (XMVectorGetX(Dot) < 0.0f)
//		{
//			outside = true;
//		}
//	}
//	bool outsideOri = false;
//	XMVECTOR VecOri[3];
//	for (int i = 0; i < 3; ++i)
//	{
//		VecOri[i] = XMVectorSubtract(SphereCenterOri, Vertex[i]);
//		XMVECTOR Cross = XMVector3Cross(Edge[i], VecOri[i]);
//		XMVECTOR Dot = XMVector3Dot(Normal, Cross);
//		if (XMVectorGetX(Dot) < 0.0f)
//		{
//			outsideOri = true;
//		}
//	}
//
//	return false;
//}

//bool MapCollider::RayVsPorigon(
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	Porigon* porigon,
//	HitResult& result
//)
//{
//	XMVECTOR Start = XMLoadFloat3(&start);
//	XMVECTOR End = XMLoadFloat3(&end);
//	XMVECTOR rayVec = XMVectorSubtract(End, Start);
//	float length = XMVectorGetX(XMVector3Length(rayVec));
//
//	// 内積の結果がプラスならば裏向き
//	XMVECTOR N = XMLoadFloat3(&porigon->normal);
//	float dot = XMVectorGetX(XMVector3Dot(rayVec, N));
//	if (dot >= 0.0f)	return false;
//
//	// レイの始点から平面の交点までの距離を算出
//	XMVECTOR A = XMLoadFloat3(&porigon->position[0]);
//	XMVECTOR B = XMLoadFloat3(&porigon->position[1]);
//	XMVECTOR C = XMLoadFloat3(&porigon->position[2]);
//	XMVECTOR SA = XMVectorSubtract(A, Start);
//	float x = XMVectorGetX(XMVector3Dot(SA, N)) / dot;
//	float distance = length * x;
//
//	// 交点までの距離が今までに計算した最近距離より
//	// 大きい時はスキップ
//	if (distance < 0.0f || distance > result.distance) return false;
//
//	// 平面上の交点P
//	XMVECTOR P = XMVectorAdd(Start, XMVectorScale(rayVec, x));
//
//	// 交点が三角形の内側にあるか判定
//	// １つめ
//	XMVECTOR Cross1 = XMVector3Cross(XMVectorSubtract(A, P), XMVectorSubtract(B, A));
//	float Dot1 = XMVectorGetX(XMVector3Dot(rayVec, Cross1));
//	if (Dot1 > 0.0f) return false;
//	// ２つめ
//	XMVECTOR Cross2 = XMVector3Cross(XMVectorSubtract(B, P), XMVectorSubtract(C, B));
//	float Dot2 = XMVectorGetX(XMVector3Dot(rayVec, Cross2));
//	if (Dot2 > 0.0f) return false;
//	// ３つめ
//	XMVECTOR Cross3 = XMVector3Cross(XMVectorSubtract(C, P), XMVectorSubtract(A, C));
//	float Dot3 = XMVectorGetX(XMVector3Dot(rayVec, Cross3));
//	if (Dot3 > 0.0f) return false;
//
//	// ヒット情報保存
//	XMStoreFloat3(&result.position, P);
//	result.normal = porigon->normal;
//	result.distance = distance;
//	result.materialIndex = porigon->materialIndex;
//
//	return true;
//}

//bool MapCollider::SearchChildren(	// 子空間探索
//	int Elem,
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	HitResult& result,
//	bool& hit
//)
//{
//	// 空間外ならreturn
//	if (Elem < 0) return hit;
//
//	// 子空間が存在するなら子空間を探索
//	if ((Elem + 1) << 3 < tree.m_iCellNum)
//	{
//		int childElem = (Elem << 3) + 1;
//
//		for (int i = 0; i < 8; i++)
//		{
//			if (SearchChildren(childElem + i, start, end, result, hit))
//				break;
//		}
//	}
//
//	// この空間が存在しないならreturn
//	if (!tree.ppCellAry[Elem])	return hit;
//
//	// この空間に登録されているポリゴンとの当たり判定
//	Liner8TreeManager<Porigon>::OFT<Porigon>* oft = tree.ppCellAry[Elem]->pLatest;
//	while (oft)
//	{
//		if (RayVsPorigon(start, end, oft->m_pObject, result))
//		{
//			hit = true;
//		}
//
//		oft = oft->m_pNext;
//	}
//
//	return hit;
//}

//bool MapCollider::SearchParent(		// 親空間探索
//	int Elem,
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	HitResult& result,
//	bool& hit
//)
//{
//	// 空間外ならreturn
//	if (Elem <= 0) return hit;
//	
//	int parentElem = (Elem - 1) >> 3;
//	while (1)
//	{
//		// この空間が存在しないなら親空間に移動
//		if (!tree.ppCellAry[parentElem])
//		{
//			// ルート空間ならbreak
//			if (parentElem == 0)	break;
//			parentElem = (parentElem - 1) >> 3;
//			continue;
//		}
//
//		Liner8TreeManager<Porigon>::OFT<Porigon>* oft = tree.ppCellAry[parentElem]->pLatest;
//		while (oft)
//		{
//			if (RayVsPorigon(start, end, oft->m_pObject, result))
//			{
//				hit = true;
//			}
//
//			oft = oft->m_pNext;
//		}
//
//		// ルート空間ならbreak
//		if (parentElem == 0)	break;
//
//		// 親空間に移動
//		parentElem = (parentElem - 1) >> 3;
//	}
//
//	return hit;
//}