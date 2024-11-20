#include	"../Lighting/Light.hlsli"

// 頂点シェーダー出力データ
struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct GSInput
{
    float4 Position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

cbuffer CbScene : register(b0)
{
    float4 cameraPosition;
    row_major float4x4 ViewProjection;

    //ライト情報
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax]; // 点光源情報
    SpotLightData spotLightData[SpotLightMax]; // スポットライト情報
    int pointLightCount; // 点光源数
    int spotLightCount; // スポットライト数
    
    float timerGlobal;
    float timerTick;
}