#include "Skinning.hlsli"
#include "DefaultModel.hlsli"

VS_OUT main(
	float4 position    : POSITION,
	float3 normal      : NORMAL,
	float3 tangent     : TANGENT,
	float2 texcoord    : TEXCOORD,
	float4 color       : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES)
{
    VS_OUT vout = (VS_OUT) 0;

    position      = SkinningPosition(position, boneWeights, boneIndices);
    vout.vertex   = mul(position, viewProjection);
	vout.texcoord = texcoord;
	vout.normal   = SkinningVector(normal, boneWeights, boneIndices);
	vout.position = position.xyz;
	
	// シャドウマップで使用する情報を算出
    for (int i = 0; i < ShadowmapCount; ++i)
    {
        vout.shadowTexcoord[i] = CalcShadowTexcoord(position.xyz, lightViewProjection[i]);
    }
	
	return vout;
}
