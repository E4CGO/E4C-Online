struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
};

cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[256];
};

cbuffer CbMaterial : register(b2)
{
    float4 materialColor;
};
