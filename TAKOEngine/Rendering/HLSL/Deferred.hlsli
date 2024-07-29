#include "Light.hlsli"
#include "Sprite.hlsli"

cbuffer CbScene : register(b0)
{
    float4               cameraPosition;
    row_major float4x4   viewProjection;
    float4               ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData       pointLightData[PointLightMax]; // 点光源情報
    SpotLightData        spotLightData[SpotLightMax];   // スポットライト情報
    int                  pointLightCount;               // 点光源数
    int                  spotLightCount;                // スポットライト数
    float2               dummy2;
}

