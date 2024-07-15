#include "Light.hlsli"
#include "ShaderData.hlsli"

struct VS_OUT
{
	float4 vertex   : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
	float3 position : POSITION;
	float4 color    : COLOR;
    float3 shadowTexcoord[ShadowmapCount] : TEXCOORD1; //シャドウマップから情報を取得するためのUV座標+深度値
};

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
    float4 ambientLightColor;
    DirectionalLightData directionalLightData; //平行光源
};

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
    float linearGamma;
    float3 dammy;
    ShaderData shaderData;
};