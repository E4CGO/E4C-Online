#include "Billboard.hlsli"

// 頂点シェーダーエントリポイント
GSInput main(
	float4 position : POSITION,
	float4 boneWeights : BONE_WEIGHTS,
	uint4 boneIndices : BONE_INDICES,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float3 normal : NORMAL,
	float3 tangent : TANGENT)

{
    GSInput output;
	
    output.Position = mul(position, WorldMatrix);
    output.texcoord = texcoord;
    output.Color = color;
	
    return output;
}