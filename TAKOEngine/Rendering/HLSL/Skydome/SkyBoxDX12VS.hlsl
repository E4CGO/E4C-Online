#include "SkyBoxDX12.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(
	float4 position    : POSITION,
	float4 boneWeights : BONE_WEIGHTS,
	uint4  boneIndices : BONE_INDICES,
	float2 texcoord    : TEXCOORD,
	float4 color       : COLOR,
	float3 normal      : NORMAL,
	float3 tangent     : TANGENT)
{
	float3 p = position.xyz;
    
    VS_OUT vout;
    float4x4 viewProjection = mul(view, Projection);
    vout.vertex   = mul(float4(p, 1.0f), mul(world_transform, viewProjection));
    vout.texcoord = texcoord;
    vout.normal   = normal;
    vout.position = position.xyz;
    vout.tangent  = tangent;
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    vout.color    = materialColor;

    return vout;
}