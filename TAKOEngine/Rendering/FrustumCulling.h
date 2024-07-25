#pragma once

#include <DirectXCollision.h>
#include <vector>
#include "Model.h"
#include "TAKOEngine\Editor\Camera\Camera.h"

struct Frustum
{
    DirectX::XMFLOAT4 planes[6]; // 6�̕��ʁi���A�E�A��A���A�߁A���j
};

class FrustumCulling
{
public:
    //������J�����O���s���֐�
    static void FrustumCullingFlag(const Camera& camera, const std::vector<Model::Mesh>& meshes, std::vector<bool>& visibleObjects);

private:

    // �t���X�^�����v�Z����֐�
    static void CalculateFrustumFromViewProjection(Frustum& frustum, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix);

    // �t���X�^���Ƃ̏Փ˔�����s���֐�
    static bool IsObjectInFrustum(const Frustum& frustum, const DirectX::BoundingBox& objectBounds);
};