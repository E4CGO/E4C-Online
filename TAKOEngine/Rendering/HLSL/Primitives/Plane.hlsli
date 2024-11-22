#include	"../Lighting/Light.hlsli"

// 頂点シェーダー出力データ
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
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