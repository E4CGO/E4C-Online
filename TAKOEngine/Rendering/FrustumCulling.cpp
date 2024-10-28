//! @file FrustumCulling.cpp
//! @note

#include "FrustumCulling.h"

//*********************************************************************
// @brief      視錐台カリングを行う関数
// @param[in]  camera          カメラ情報
// @param[in]  meshes          メッシュデータのリスト
// @param[in]  visibleObjects  メッシュが視錐台内にあるかどうかのフラグを格納するブール値のリスト
// @return     なし
//*********************************************************************
void FrustumCulling::FrustumCullingFlag(const std::vector<iModel::Mesh>& meshes, std::vector<bool>& visibleObjects)
{
    // カメラのビュー行列と射影行列を取得
    DirectX::XMFLOAT4X4 viewMatrix       = CameraManager::Instance().GetCamera()->GetView();
    DirectX::XMFLOAT4X4 projectionMatrix = CameraManager::Instance().GetCamera()->GetProjection();

    // カメラの視錐台（フラスタム）を計算
    Frustum frustum;
    CalculateFrustumFromViewProjection(frustum, viewMatrix, projectionMatrix);

    // 各メッシュに対してフラスタム内に収まるか判定してフラグをセット
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const iModel::Mesh&   mesh       = meshes[i];
        DirectX::BoundingBox meshBounds = mesh.worldBounds;
        visibleObjects[i] = IsObjectInFrustum(frustum, meshBounds);
    }
}

//*********************************************************************
// @brief      フラスタムを計算する関数
// @param[in]  frustum           結果として計算される視錐台（フラスタム）情報を格納するためのオブジェクト
// @param[in]  viewMatrix        ビュー行列
// @param[in]  projectionMatrix  カメラの視野角やクリップ距離（近接・遠方クリップ面）を定義するための行列
// @return     なし
//*********************************************************************
void FrustumCulling::CalculateFrustumFromViewProjection(Frustum& frustum, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix)
{
    // ビュー行列と射影行列を組み合わせた行列を計算
    DirectX::XMFLOAT4X4 viewProjectionMatrix;
    DirectX::XMStoreFloat4x4(&viewProjectionMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&viewMatrix), DirectX::XMLoadFloat4x4(&projectionMatrix)));

    // 左平面
    frustum.planes[0].x = viewProjectionMatrix._14 + viewProjectionMatrix._11;
    frustum.planes[0].y = viewProjectionMatrix._24 + viewProjectionMatrix._21;
    frustum.planes[0].z = viewProjectionMatrix._34 + viewProjectionMatrix._31;
    frustum.planes[0].w = viewProjectionMatrix._44 + viewProjectionMatrix._41;

    // 右平面
    frustum.planes[1].x = viewProjectionMatrix._14 - viewProjectionMatrix._11;
    frustum.planes[1].y = viewProjectionMatrix._24 - viewProjectionMatrix._21;
    frustum.planes[1].z = viewProjectionMatrix._34 - viewProjectionMatrix._31;
    frustum.planes[1].w = viewProjectionMatrix._44 - viewProjectionMatrix._41;

    // 上平面
    frustum.planes[2].x = viewProjectionMatrix._14 - viewProjectionMatrix._12;
    frustum.planes[2].y = viewProjectionMatrix._24 - viewProjectionMatrix._22;
    frustum.planes[2].z = viewProjectionMatrix._34 - viewProjectionMatrix._32;
    frustum.planes[2].w = viewProjectionMatrix._44 - viewProjectionMatrix._42;

    // 下平面
    frustum.planes[3].x = viewProjectionMatrix._14 + viewProjectionMatrix._12;
    frustum.planes[3].y = viewProjectionMatrix._24 + viewProjectionMatrix._22;
    frustum.planes[3].z = viewProjectionMatrix._34 + viewProjectionMatrix._32;
    frustum.planes[3].w = viewProjectionMatrix._44 + viewProjectionMatrix._42;

    // 近平面
    frustum.planes[4].x = viewProjectionMatrix._13;
    frustum.planes[4].y = viewProjectionMatrix._23;
    frustum.planes[4].z = viewProjectionMatrix._33;
    frustum.planes[4].w = viewProjectionMatrix._43;

    // 遠平面
    frustum.planes[5].x = viewProjectionMatrix._14 - viewProjectionMatrix._13;
    frustum.planes[5].y = viewProjectionMatrix._24 - viewProjectionMatrix._23;
    frustum.planes[5].z = viewProjectionMatrix._34 - viewProjectionMatrix._33;
    frustum.planes[5].w = viewProjectionMatrix._44 - viewProjectionMatrix._43;

    // 平面を正規化
    for (int i = 0; i < 6; ++i)
    {
        DirectX::XMVECTOR planeVec = DirectX::XMVectorSet(frustum.planes[i].x, frustum.planes[i].y, frustum.planes[i].z, frustum.planes[i].w);
        planeVec = DirectX::XMPlaneNormalize(planeVec);
        DirectX::XMStoreFloat4(&frustum.planes[i], planeVec);
    }
}

//*********************************************************************
// @brief      フラスタムとの衝突判定を行う関数
// @param[in]  frustum      フラスタム（視錐台）を表すオブジェクト
// @param[in]  objectBounds オブジェクトの境界ボックス
// @return     bool
//*********************************************************************
bool FrustumCulling::IsObjectInFrustum(const Frustum& frustum, const DirectX::BoundingBox& objectBounds)
{
    // オブジェクトの境界ボックスの頂点を取得
    DirectX::XMFLOAT3 corners[8];
    objectBounds.GetCorners(corners);

    // フラスタムの各平面との比較を行う
    for (int i = 0; i < 6; ++i)
    {
        DirectX::XMFLOAT4 plane  = frustum.planes[i];
        DirectX::XMVECTOR normal = DirectX::XMVectorSet(plane.x, plane.y, plane.z, 0.0f);
        DirectX::XMVECTOR origin = DirectX::XMVectorScale(normal, -plane.w);

        // オブジェクトの頂点をフラスタムの平面と比較
        bool allOutside = true;
        for (int j = 0; j < 8; ++j)
        {
            DirectX::XMVECTOR vertex = DirectX::XMLoadFloat3(&corners[j]);
            float distance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(vertex, normal)) + plane.w;
            if (distance >= 0.0f)
            {
                allOutside = false;
                break;
            }
        }

        // オブジェクトがフラスタムの外側にあれば非表示と判定
        if (allOutside)
        {
            return false;
        }
    }

    // すべての平面との比較を通過した場合、オブジェクトはフラスタム内にある
    return true;
}