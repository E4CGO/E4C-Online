#include	"../Lighting/Light.hlsli"

// ���_�V�F�[�_�[�o�̓f�[�^
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    float4 cameraPosition;
    row_major float4x4 lightViewProjection;
    
    //���C�g���
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax]; // �_�������
    SpotLightData spotLightData[SpotLightMax]; // �X�|�b�g���C�g���
    int pointLightCount; // �_������
    int spotLightCount; // �X�|�b�g���C�g��
};

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