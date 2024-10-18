#include	"../Lighting/Light.hlsli"

struct VS_OUT
{
    float4 vertex	: SV_POSITION;
	float2 texcoord	: TEXCOORD;
	float3 normal	: NORMAL;
	float3 position	: POSITION;
	float3 tangent	: TANGENT;
    float3 binormal : BINORMAL;
    float4 color    : COLOR;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    
    //ライト情報
    float4               ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData       pointLightData[PointLightMax]; 	// 点光源情報
    SpotLightData        spotLightData[SpotLightMax];   	// スポットライト情報
    int                  pointLightCount;               	// 点光源数
    int                  spotLightCount;                	// スポットライト数
};

cbuffer CbMesh : register(b1)
{
    row_major float4x4 world_transform;
    row_major float4x4 boneTransforms[255];
};

cbuffer CbMaterial : register(b2)
{
    float4 materialColor;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(CBV(b1), visibility=SHADER_VISIBILITY_VERTEX), \
                  DescriptorTable(CBV(b2), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(SRV(t1), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(Sampler(s0), visibility=SHADER_VISIBILITY_PIXEL)"
