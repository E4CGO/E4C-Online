struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

cbuffer CbMesh : register(b0)
{
	row_major float4x4	worldViewProjection;
	float4				color;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_VERTEX)"
