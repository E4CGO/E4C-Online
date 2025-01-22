struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	row_major float4x4	wvp;
	float4				color;
};
