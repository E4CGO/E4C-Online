cbuffer CbSkeleton : register(b2)
{
	row_major float4x4 boneTransforms[256];
}

float4 SkinningPosition(float4 position, float4 boneWeights, uint4 boneIndices)
{
	float4 p = float4(0, 0, 0, 0);

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]]));
	}
	return p;
}

float3 SkinningVector(float3 vec, float4 boneWeights, uint4 boneIndices)
{
	float3 v = float3(0, 0, 0);

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		v += boneWeights[i] * mul(float4(vec, 0), boneTransforms[boneIndices[i]]).xyz;
	}
	return v;
}