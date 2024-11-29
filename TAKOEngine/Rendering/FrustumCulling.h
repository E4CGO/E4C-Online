//! @file FrustumCulling.h
//! @note

#ifndef __MATH_FRUSTUM_CULLING_H__
#define __MATH_FRUSTUM_CULLING_H__

#include <DirectXCollision.h>
#include <vector>
#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include "TAKOEngine/Editor/Camera/Camera.h"

//*************************************************************
// @class FrustumCulling
// @brief 視錐台カリング
// @par   [説明]
//		  視錐台カリングの計算を行う
//*************************************************************
class FrustumCulling
{
public:
	//視錐台カリングを行う関数
	static void FrustumCullingFlag(const std::vector<iModel::Mesh>& meshes, std::vector<bool>& visibleObjects);

private:
	struct Frustum
	{
		DirectX::XMFLOAT4 planes[6]; // 6つの平面（左、右、上、下、近、遠）
	};

	// フラスタムを計算する関数
	static void CalculateFrustumFromViewProjection(Frustum& frustum, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);

	// フラスタムとの衝突判定を行う関数
	static bool IsObjectInFrustum(const Frustum& frustum, const DirectX::BoundingBox& objectBounds);
};

#endif // !__MATH_FRUSTUM_CULLING_H__