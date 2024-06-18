struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
};

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
    float linearGamma;
};