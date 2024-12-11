//! @file Collision.h
//! @note 当たり判定処理総まとめ
 
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <DirectXMath.h>
#include <cmath>
#include <algorithm>
#include "TAKOEngine/Rendering/Model/Model.h"

// ヒット結果
struct HitResult
{
	DirectX::XMFLOAT3	position = { 0, 0, 0 };// レイとポリゴンの交点
	DirectX::XMFLOAT3	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル
	float				distance = FLT_MAX; 		// レイの始点から交点までの距離
	int					materialIndex = -1; 	// 衝突したポリゴンのマテリアル番号
	DirectX::XMFLOAT3	triangleVerts[3] = {};
};
struct HitResultVector
{
	DirectX::XMVECTOR	position = { 0, 0, 0 };// レイとポリゴンの交点
	DirectX::XMVECTOR	normal = { 0, 0, 0 };	// 衝突したポリゴンの法線ベクトル
	float				distance = 0.0f; 		// レイの始点から交点までの距離
	DirectX::XMVECTOR	triangleVerts[3] = {};
	int					materialIndex = -1; 	// 衝突したポリゴンのマテリアル番号
};
// オブジェクトの交差結果の構造体
struct IntersectionResult
{
	DirectX::XMVECTOR	pointA = {};			// オブジェクトA側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	pointB = {};			// オブジェクトB側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	normal = {};			// 交点を結ぶ衝突の単位法線ベクトル。方向はオブジェクトB→オブジェクトA
	float				penetration = 0.0f; 	// 法線ベクトルを元にしためり込み量。交差している場合にプラスの値が返却される
};

struct Triangle
{
	DirectX::XMFLOAT3	position[3] = {};
	int					materialIndex = -1;

	// 三角形に外接するAABBを構成する最小点と最大点を取得する
	void GetBoundPoints(DirectX::XMFLOAT3* minPoint, DirectX::XMFLOAT3* maxPoint)
	{
		if (minPoint && maxPoint)
		{
			minPoint->x = min(min(position[0].x, position[1].x), position[2].x);
			minPoint->y = min(min(position[0].y, position[1].y), position[2].y);
			minPoint->z = min(min(position[0].z, position[1].z), position[2].z);
			maxPoint->x = max(max(position[0].x, position[1].x), position[2].x);
			maxPoint->y = max(max(position[0].y, position[1].y), position[2].y);
			maxPoint->z = max(max(position[0].z, position[1].z), position[2].z);
		}
	}
};
struct Sphere
{
	DirectX::XMFLOAT3	position;	// 中心
	float				radius;	// 半径

	// 球体に外接するAABBを構成する最小点と最大点を取得する
	void GetBoundPoints(DirectX::XMFLOAT3* minPoint, DirectX::XMFLOAT3* maxPoint)
	{
		if (minPoint && maxPoint)
		{
			*minPoint = {
				position.x - radius,
				position.y - radius,
				position.z - radius
			};

			*maxPoint = {
				position.x + radius,
				position.y + radius,
				position.z + radius
			};
		}
	}
};
struct AABB
{
	DirectX::XMFLOAT3	position;	// 中心
	DirectX::XMFLOAT3	radii;	// 半径
};
struct OBB
{
	DirectX::XMFLOAT3	position;	// 中心
	DirectX::XMFLOAT3	radii;	// 反辺長
	DirectX::XMFLOAT4	orientation;	// 姿勢
};
struct Capsule
{
	DirectX::XMFLOAT3	position;	// 中心
	DirectX::XMFLOAT3	direction;	// 向き（正規化）
	float				length;	// 長さ
	float				radius;	// 半径

	// カプセルに外接するAABBを構成する最小点と最大点を取得する
	void GetBoundPoints(DirectX::XMFLOAT3* minPoint, DirectX::XMFLOAT3* maxPoint)
	{
		if (minPoint && maxPoint)
		{
			if (direction.x > 0)
			{
				minPoint->x = position.x - radius;
				maxPoint->x = position.x + direction.x * length + radius;
			}
			else
			{
				minPoint->x = position.x + direction.x * length - radius;
				maxPoint->x = position.x + radius;
			}

			if (direction.y > 0)
			{
				minPoint->y = position.y - radius;
				maxPoint->y = position.y + direction.y * length + radius;
			}
			else
			{
				minPoint->y = position.y + direction.y * length - radius;
				maxPoint->y = position.y + radius;
			}

			if (direction.z > 0)
			{
				minPoint->z = position.z - radius;
				maxPoint->z = position.z + direction.z * length + radius;
			}
			else
			{
				minPoint->z = position.z + direction.z * length - radius;
				maxPoint->z = position.z + radius;
			}
		}
	}
};

/**************************************************************************//**
	@class		Collision
	@brief		当たり判定処理
	@par		staticクラス
*//***************************************************************************/
class Collision
{
public:
	// レイとモデルの交差判定
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const iModel* model,
		HitResultVector& result
	);

	// レイと三角形の交差判定
	static bool IntersectRayVsTriangle(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMVECTOR triangleVerts[3],
		HitResultVector& result);

	static bool IntersectRayVsTriangle(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 非正規化
		const DirectX::XMVECTOR triangleVerts[3],
		HitResultVector& result);

	static bool IntersectRayVsTriangle(
		const DirectX::XMFLOAT3& rayStart,
		const DirectX::XMFLOAT3& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMFLOAT3 triangleVerts[3],
		HitResult& result);

	// 外部の点に対する三角形内部の最近点を取得する
	static bool GetClosestPoint_PointTriangle(
		const DirectX::XMVECTOR& point,
		const DirectX::XMVECTOR trianglePos[3],
		DirectX::XMVECTOR& nearPos);

	// 球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMVECTOR& position1,
		float radius1,
		const DirectX::XMVECTOR& position2,
		float radius2,
		IntersectionResult* result);
	
	// 球Vs三角形
	static bool IntersectSphereVsTriangle(
		const DirectX::XMVECTOR& spherePos,
		float radius,
		const DirectX::XMVECTOR trianglePos[3],
		IntersectionResult* result = {},
		bool triangleNormOnly = false);
	
	// 球VsAABB
	static bool IntersectSphereVsAABB(
		const DirectX::XMVECTOR& spherePos,
		float sphereRadius,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		IntersectionResult* result);
	
	// 球vsカプセルの交差判定
	static bool IntersectSphereVsCapsule(
		const DirectX::XMVECTOR& spherePos,
		float sphereRadius,
		const DirectX::XMVECTOR& capsulePos,
		const DirectX::XMVECTOR& capsuleDirection,
		float capsuleLength,
		float capsuleRadius,					// カプセルの半径
		IntersectionResult* result = {});

	// AABBと平面の最短距離を取得する	※返り値がマイナスだと交差している状況と定義
	static float GetMinDist_AABBPlane(
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const DirectX::XMVECTOR& planeNorm,	// 要正規化
		const float planeDist,
		bool* reverse);				// 法線に対して裏か表かを判断するようのフラグ

	// AABBvs三角形の交差判定
	static bool IntersectAABBVsTriangle(
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		const DirectX::XMVECTOR trianglePos[3],
		IntersectionResult* result);
	
	// AABBvsAABB
	static bool IntersectAABBVsAABB(
		const DirectX::XMVECTOR& aabbPos1,
		const DirectX::XMVECTOR& aabbRadii1,
		const DirectX::XMVECTOR& aabbPos2,
		const DirectX::XMVECTOR& aabbRadii2,
		IntersectionResult* result);

	// 線分と線分の最短距離の二乗を取得する
	static float GetMinDistSq_SegmentSegment(
		const  DirectX::XMVECTOR& point1A,
		const  DirectX::XMVECTOR& point1B,
		const  DirectX::XMVECTOR& point2A,
		const  DirectX::XMVECTOR& point2B,
		DirectX::XMVECTOR* nearPoint1,
		DirectX::XMVECTOR* nearPoint2);

	// 線分と三角形の最短距離の二乗を取得する
	static float GetMinDistSq_SegmentTriangle(
		const  DirectX::XMVECTOR& pointA,
		const  DirectX::XMVECTOR& pointB,
		const  DirectX::XMVECTOR trianglePos[3],
		DirectX::XMVECTOR* nearPointSegment,
		DirectX::XMVECTOR* nearPointTriangle,
		bool& nearPointEdghFlg);

	// カプセルvs三角形
	static bool IntersectCapsuleVsTriangle(
		const DirectX::XMVECTOR& position,
		const DirectX::XMVECTOR& direction,
		float radius,
		float length,
		const DirectX::XMVECTOR trianglePos[3],
		IntersectionResult* result);

	// カプセルvsカプセルの交差判定
	static bool IntersectCapsuleVsCapsule(
		const DirectX::XMVECTOR& position1,		// 中心
		const DirectX::XMVECTOR& direction1,	// 向き（正規化）
		const float					length1,	// 長さ
		const float					radius1,	// 半径
		const DirectX::XMVECTOR& position2,		// 中心
		const DirectX::XMVECTOR& direction2,	// 向き（正規化）
		const float					length2,	// 長さ
		const float					radius2,	// 半径
		IntersectionResult* result);

	// レイVsスラブ3D (軸平行スラブ)
	static bool IntersectRayVsSlub3D(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMVECTOR& slubCenter,
		const DirectX::XMVECTOR& slubRadii,
		HitResultVector* resultNear = {},			// レイとスラブの最初の交点情報
		HitResultVector* resultFar = {});			// レイとスラブの最後の交点情報

	// レイVsAABB
	static bool IntersectRayVsAABB(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		HitResultVector* resultNear,
		HitResultVector* resultFar);

	// 外部の点に対するAABB内部の最近点を取得する
	static DirectX::XMVECTOR GetClosestPoint_PointAABB(
		const DirectX::XMVECTOR& point,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		bool surfaceFlg = false);

	// 線分とAABBの最短距離の二乗を取得する
	static float GetMinDistSq_SegmentAABB(
		const DirectX::XMVECTOR& pointA,
		const DirectX::XMVECTOR& pointB,
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		DirectX::XMVECTOR* nearPointSegment = {},
		DirectX::XMVECTOR* nearPointAABB = {});

	// カプセルvsAABBの交差判定				※カプセル中心間のレイがAABBと交差している場合の押し戻しのためのresultはレイ方向に動く仕様。レイ以外の最短方向ではない。
	static bool IntersectCapsuleVsAABB(
		const DirectX::XMVECTOR& position,	// 中心
		const DirectX::XMVECTOR& direction,	// 向き（正規化）
		const float				length,	// 長さ
		const float				radius,	// 半径
		const DirectX::XMVECTOR& aabbPos,
		const DirectX::XMVECTOR& aabbRadii,
		IntersectionResult* result = {});

	// レイVs球
	static bool IntersectRayVsSphere(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMVECTOR& spherePos,
		float radius,
		HitResultVector* result = {});

	// レイVs円柱
	static bool IntersectRayVsOrientedCylinder(
		const DirectX::XMVECTOR& rayStart,
		const DirectX::XMVECTOR& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMVECTOR& startCylinder,
		const DirectX::XMVECTOR& endCylinder,
		float radius,
		HitResultVector* result = {},
		DirectX::XMVECTOR* onCenterLinPos = {});

	// スフィアキャストVs三角形
	static bool IntersectSphereCastVsTriangle(
		const DirectX::XMVECTOR& sphereCastStart,		// スフィアキャストの発射点（球の中心）
		const DirectX::XMVECTOR& sphereCastDirection,	// スフィアキャストの向き。要正規化
		float sphereCastDist,
		float sphereCastRadius,							// スフィアキャストの半径
		const DirectX::XMVECTOR trianglePos[3],
		HitResult* result = {},
		bool firstSphereChk = false);				// スフィアキャストの発射点の球が既に三角形に接している場合にfalseを返すチェックを行うかどうか

	// スフィアキャストVsモデル（メッシュ）
	static bool IntersectSphereCastVsModel(
		const DirectX::XMFLOAT3& start,			// スフィアキャストのスタートの位置の球の中心
		const DirectX::XMFLOAT3& end,  			// スフィアキャストのエンドの位置の球の中心
		float radius,
		const iModel* model,
		HitResultVector* result = {});
	static bool IntersectSphereCastVsModel(
		const DirectX::XMVECTOR& start,			// スフィアキャストのスタートの位置の球の中心
		const DirectX::XMVECTOR& end,  			// スフィアキャストのエンドの位置の球の中心
		float radius,
		const iModel* model,
		HitResultVector* result = {});
};

#endif