#include "ToonDX12.hlsli"

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
#if 0
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }
#else
	float3 p = position.xyz;
#endif
    
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