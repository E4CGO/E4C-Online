#include "FrustumCulling.h"

// ������J�����O���s���֐�
void FrustumCulling::FrustumCullingFlag(const Camera & camera, const std::vector<Model::Mesh>&meshes, std::vector<bool>&visibleObjects)
{
    // �J�����̃r���[�s��Ǝˉe�s����擾
    DirectX::XMFLOAT4X4 viewMatrix       = camera.GetView();
    DirectX::XMFLOAT4X4 projectionMatrix = camera.GetProjection();

    // �J�����̎�����i�t���X�^���j���v�Z
    Frustum frustum;
    CalculateFrustumFromViewProjection(frustum, viewMatrix, projectionMatrix);

    // �e���b�V���ɑ΂��ăt���X�^�����Ɏ��܂邩���肵�ăt���O���Z�b�g
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const Model::Mesh&   mesh       = meshes[i];
        DirectX::BoundingBox meshBounds = mesh.worldBounds;
        visibleObjects[i] = IsObjectInFrustum(frustum, meshBounds);
    }
}

// �t���X�^�����v�Z����֐�
void FrustumCulling::CalculateFrustumFromViewProjection(Frustum& frustum, const DirectX::XMFLOAT4X4& viewMatrix, const DirectX::XMFLOAT4X4& projectionMatrix)
{
    // �r���[�s��Ǝˉe�s���g�ݍ��킹���s����v�Z
    DirectX::XMFLOAT4X4 viewProjectionMatrix;
    DirectX::XMStoreFloat4x4(&viewProjectionMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&viewMatrix), DirectX::XMLoadFloat4x4(&projectionMatrix)));

    // ������
    frustum.planes[0].x = viewProjectionMatrix._14 + viewProjectionMatrix._11;
    frustum.planes[0].y = viewProjectionMatrix._24 + viewProjectionMatrix._21;
    frustum.planes[0].z = viewProjectionMatrix._34 + viewProjectionMatrix._31;
    frustum.planes[0].w = viewProjectionMatrix._44 + viewProjectionMatrix._41;

    // �E����
    frustum.planes[1].x = viewProjectionMatrix._14 - viewProjectionMatrix._11;
    frustum.planes[1].y = viewProjectionMatrix._24 - viewProjectionMatrix._21;
    frustum.planes[1].z = viewProjectionMatrix._34 - viewProjectionMatrix._31;
    frustum.planes[1].w = viewProjectionMatrix._44 - viewProjectionMatrix._41;

    // �㕽��
    frustum.planes[2].x = viewProjectionMatrix._14 - viewProjectionMatrix._12;
    frustum.planes[2].y = viewProjectionMatrix._24 - viewProjectionMatrix._22;
    frustum.planes[2].z = viewProjectionMatrix._34 - viewProjectionMatrix._32;
    frustum.planes[2].w = viewProjectionMatrix._44 - viewProjectionMatrix._42;

    // ������
    frustum.planes[3].x = viewProjectionMatrix._14 + viewProjectionMatrix._12;
    frustum.planes[3].y = viewProjectionMatrix._24 + viewProjectionMatrix._22;
    frustum.planes[3].z = viewProjectionMatrix._34 + viewProjectionMatrix._32;
    frustum.planes[3].w = viewProjectionMatrix._44 + viewProjectionMatrix._42;

    // �ߕ���
    frustum.planes[4].x = viewProjectionMatrix._13;
    frustum.planes[4].y = viewProjectionMatrix._23;
    frustum.planes[4].z = viewProjectionMatrix._33;
    frustum.planes[4].w = viewProjectionMatrix._43;

    // ������
    frustum.planes[5].x = viewProjectionMatrix._14 - viewProjectionMatrix._13;
    frustum.planes[5].y = viewProjectionMatrix._24 - viewProjectionMatrix._23;
    frustum.planes[5].z = viewProjectionMatrix._34 - viewProjectionMatrix._33;
    frustum.planes[5].w = viewProjectionMatrix._44 - viewProjectionMatrix._43;

    // ���ʂ𐳋K��
    for (int i = 0; i < 6; ++i)
    {
        DirectX::XMVECTOR planeVec = DirectX::XMVectorSet(frustum.planes[i].x, frustum.planes[i].y, frustum.planes[i].z, frustum.planes[i].w);
        planeVec = DirectX::XMPlaneNormalize(planeVec);
        DirectX::XMStoreFloat4(&frustum.planes[i], planeVec);
    }
}

// �t���X�^���Ƃ̏Փ˔�����s���֐�
bool FrustumCulling::IsObjectInFrustum(const Frustum& frustum, const DirectX::BoundingBox& objectBounds)
{
    // �I�u�W�F�N�g�̋��E�{�b�N�X�̒��_���擾
    DirectX::XMFLOAT3 corners[8];
    objectBounds.GetCorners(corners);

    // �t���X�^���̊e���ʂƂ̔�r���s��
    for (int i = 0; i < 6; ++i)
    {
        DirectX::XMFLOAT4 plane  = frustum.planes[i];
        DirectX::XMVECTOR normal = DirectX::XMVectorSet(plane.x, plane.y, plane.z, 0.0f);
        DirectX::XMVECTOR origin = DirectX::XMVectorScale(normal, -plane.w);

        // �I�u�W�F�N�g�̒��_���t���X�^���̕��ʂƔ�r
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

        // �I�u�W�F�N�g���t���X�^���̊O���ɂ���Δ�\���Ɣ���
        if (allOutside)
        {
            return false;
        }
    }

    // ���ׂĂ̕��ʂƂ̔�r��ʉ߂����ꍇ�A�I�u�W�F�N�g�̓t���X�^�����ɂ���
    return true;
}