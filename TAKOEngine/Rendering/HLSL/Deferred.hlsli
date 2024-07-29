#include "Light.hlsli"
#include "Sprite.hlsli"

cbuffer CbScene : register(b0)
{
    float4               cameraPosition;
    row_major float4x4   viewProjection;
    float4               ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData       pointLightData[PointLightMax]; // �_�������
    SpotLightData        spotLightData[SpotLightMax];   // �X�|�b�g���C�g���
    int                  pointLightCount;               // �_������
    int                  spotLightCount;                // �X�|�b�g���C�g��
    float2               dummy2;
}

