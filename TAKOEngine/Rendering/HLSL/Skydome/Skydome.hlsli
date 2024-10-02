struct VS_OUT
{
    float4 vertex : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
}

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
    float linearGamma;
}