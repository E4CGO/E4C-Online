#include "LambertDX12.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
#if 1
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
    }
#else
	float3 p = position.xyz;
	float3 n = normal;
#endif
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);

    float3 N = normalize(n);
    float3 L = normalize(-lightDirection.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;
    vout.color.rgb = color.rgb * power;
    vout.color.a = color.a;
    vout.texcoord = texcoord;

    return vout;
}
