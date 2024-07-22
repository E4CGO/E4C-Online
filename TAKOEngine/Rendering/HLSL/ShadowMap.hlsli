#include "ShadowmapFunctions.hlsli"

//シャドウマップの枚数
static const int ShadowmapCount = 4;

Texture2D shadowMap[ShadowmapCount] : register(t10);
SamplerState shadowSampler : register(s10);

cbuffer CbShadowmap : register(b3)
{
	row_major float4x4 lightViewProjection[ShadowmapCount];  //ライトビュープロジェクション行列
	float4             shadowBias;			                 //深度値比較時のオフセット
	float3             shadowColor;			                 //影の色
    float              dummy;
}