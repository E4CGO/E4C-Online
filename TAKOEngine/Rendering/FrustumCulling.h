#pragma once

#include <DirectXCollision.h>
#include <vector>
#include "Model.h"
#include "TAKOEngine\Editor\Camera\Camera.h"

struct Frustum
{
    DirectX::XMFLOAT4 planes[6]; // 6つの平面（左、右、上、下、近、遠）
};

class FrustumCulling
{
public:
    //視錐台カリングを行う関数
    static void FrustumCullingFlag(const Camera& camera, const std::vector<Model::Mesh>& meshes, std::vector<bool>& visibleObjects);

private:

    // フラスタムを計算する関数
    static void CalculateFrustumFromViewProjection(Frustum& frustum, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);

    // フラスタムとの衝突判定を行う関数
    static bool IsObjectInFrustum(const Frustum& frustum, const DirectX::BoundingBox& objectBounds);
};