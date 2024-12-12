#include "LambertDX12.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(
	float4 position    : POSITION,
	float4 boneWeights : BONE_WEIGHTS,
	uint4  boneIndices : BONE_INDICES,
	float2 texcoord    : TEXCOORD,
	float4 color       : COLOR,
	float3 normal      : NORMAL,
	float3 tangent     : TANGENT,
    uint   instanceId  : SV_InstanceID)
{
	float3 p = position.xyz;
    float3 n = normal.xyz;
    float3 t = tangent.xyz;
    p = mul(float4(p, 1), instancingTransform[instanceId]).xyz;
    n = mul(float4(n, 0), instancingTransform[instanceId]).xyz;
    t = mul(float4(t, 0), instancingTransform[instanceId]).xyz;
    
    VS_OUT vout;
    float4x4 viewProjection = mul(view, Projection);
    vout.vertex   = mul(float4(p, 1.0f), mul(world_transform, viewProjection));
    vout.texcoord = texcoord;
    vout.normal   = normalize(n);
    vout.position = p;
    vout.tangent  = normalize(t);
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    vout.color    = materialColor;
    vout.shadow   = CalcShadowTexcoord(position.xyz, lightViewProjection);

    return vout;
}