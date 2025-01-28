#include "../Lighting/Light.hlsli"
#include "Constants.hlsli"

struct VS_OUT
{
    float4 position : POSITION;
};
typedef VS_OUT VS_CONTROL_POINT_OUTPUT;
typedef VS_OUT HS_CONTROL_POINT_OUTPUT;

struct HS_CONSTANT_DATA_OUTPUT
{
    float tess_factor[3]     : SV_TessFactor;
    float inside_tess_factor : SV_InsideTessFactor;
};
#define CONTROL_POINT_COUNT 3

struct DS_OUT
{
    float4 position : POSITION;
};

struct GS_OUT
{
    float4 sv_position : SV_POSITION;
    float4 position    : POSITION;
    float4 normal      : NORMAL;
    float2 texcoord    : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    float4             cameraPosition;
    row_major float4x4 lightViewProjection;
    
    //ライト情報
    float4               ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData       pointLightData[PointLightMax]; 	// 点光源情報
    SpotLightData        spotLightData[SpotLightMax];   	// スポットライト情報
    int                  pointLightCount;              		// 点光源数
    int                  spotLightCount;                	// スポットライト数
    
    float timerGlobal;
    float timerTick;

	// 影情報
    float shadowBias;
    float3 shadowColor;
    
    // 草情報
    float tesselation_max_subdivision;
    float m_grass_blade_height;
    float m_grass_blade_width;
    float noise_seed_multiplier;
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
                  DescriptorTable(CBV(b1), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(CBV(b2), visibility=SHADER_VISIBILITY_ALL)"