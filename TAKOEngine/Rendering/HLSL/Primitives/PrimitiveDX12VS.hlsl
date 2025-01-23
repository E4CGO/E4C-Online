#include "PrimitiveDX12.hlsli"

[RootSignature(ROOT_SIG)]
VS_OUT main(
	float4 position : POSITION,
	float4 boneWeights : BONE_WEIGHTS,
	uint4 boneIndices : BONE_INDICES,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float3 normal : NORMAL,
	float3 tangent : TANGENT)
{
    float minY = 0.0f; // Minimum vertical bound of the mesh
    float maxY = 1.0f; // Maximum vertical bound of the mesh
    
    VS_OUT vout;
    float normalizedX = (position.x - minY) / (maxY - minY);
    float normalizedY = (position.y - minY) / (maxY - minY);
    float normalizedZ = (position.z - minY) / (maxY - minY);
    vout.local_position.x = saturate(normalizedX);
    vout.local_position.y = saturate(normalizedY);
    vout.local_position.z = saturate(normalizedZ);
    
    
    float4x4 viewProjection = mul(view, Projection);
    position = mul(position, world_transform);
    vout.vertex = mul(position, viewProjection);
    vout.position = position.xyz;
    vout.texcoord = texcoord;
    vout.normal = normal;
    vout.tangent = tangent;
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    vout.color.rgb = color.rgb * materialColor.rgb;
    vout.color.a = color.a;

    vout.shadow = CalcShadowTexcoord(position.xyz, lightViewProjection);
    
    return vout;
}