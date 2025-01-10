#include	"../Lighting/Light.hlsli"

// 頂点シェーダー出力データ
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    float4 cameraPosition;
    row_major float4x4 ViewProjection;

    //ライト情報
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax]; // 点光源情報
    SpotLightData spotLightData[SpotLightMax]; // スポットライト情報
    int pointLightCount; // 点光源数
    int spotLightCount; // スポットライト数
    
    float timerGlobal;
    float timerTick;
}

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  StaticSampler(s0 ,\
                  filter = FILTER_MIN_MAG_MIP_LINEAR,\
                  addressU = TEXTURE_ADDRESS_WRAP,\
                  addressV = TEXTURE_ADDRESS_WRAP,\
                  addressW = TEXTURE_ADDRESS_WRAP,\
                  mipLodBias = 0.0f,\
                  maxAnisotropy = 0,\
                  comparisonFunc  = COMPARISON_NEVER,\
                  borderColor = STATIC_BORDER_COLOR_TRANSPARENT_BLACK,\
                  minLOD = 0.0f,\
                  maxLOD = 3.402823466e+38f,\
                  space  =  0,\
                  visibility = SHADER_VISIBILITY_PIXEL\)"