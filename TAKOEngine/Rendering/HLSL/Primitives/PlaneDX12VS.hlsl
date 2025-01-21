#include "PlaneDX12.hlsli"

// 頂点シェーダーエントリポイント
[RootSignature(ROOT_SIG)]
VS_OUT main(
	float4 position		: POSITION,
	float4 boneWeights	: BONE_WEIGHTS,
	uint4 boneIndices	: BONE_INDICES,
	float2 texcoord		: TEXCOORD,
	float4 color		: COLOR,
	float3 normal		: NORMAL,
	float3 tangent		: TANGENT)
{
    VS_OUT vout;
    float4x4 viewProjection = mul(View, Projection);
    float4 worldPosition = mul(position, WorldMatrix);
    
    vout.position = mul(worldPosition, viewProjection);
    vout.color = color;
    vout.texcoord = texcoord;

    return vout;
}