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
    position = mul(position, world_transform);
    
    VS_OUT vout;
    float4x4 viewProjection = mul(view, Projection);
    vout.vertex   = mul(position, viewProjection);
    vout.texcoord = texcoord;
    vout.normal   = normal;
    vout.position = position.xyz;
    vout.tangent  = tangent;
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    vout.color.rgb = color.rgb * materialColor.rgb;
    vout.color.a   = color.a;
    vout.shadow    = CalcShadowTexcoord(position.xyz, lightViewProjection);
    
    return vout;
}