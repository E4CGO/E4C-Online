#include "../Lighting/Light.hlsli"
#include "../Lighting/ShadowmapFunctions.hlsli"
#include "../Model/Constants.hlsli"

struct VS_OUT
{
    float4 vertex : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 position : POSITION;
    float4 local_position : LOCAL_POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color : COLOR;
    float3 shadow : SHADOW;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    float4 cameraPosition;
    row_major float4x4 lightViewProjection;
    
    //ライト情報
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax]; // 点光源情報
    SpotLightData spotLightData[SpotLightMax]; // スポットライト情報
    int pointLightCount; // 点光源数
    int spotLightCount; // スポットライト数
    
    float timerGlobal;
    float timerTick;

	// 影情報
    float shadowBias;
    float3 shadowColor;
};

cbuffer CbMesh : register(b1)
{
    row_major float4x4 world_transform;
    row_major float4x4 boneTransforms[MAX_BONES];
    row_major float4x4 instancingTransform[MAX_INSTANCES];
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
                  DescriptorTable(Sampler(s0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(SRV(t2), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(Sampler(s1), visibility=SHADER_VISIBILITY_PIXEL)"
