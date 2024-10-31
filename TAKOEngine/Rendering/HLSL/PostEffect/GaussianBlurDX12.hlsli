// ���_�V�F�[�_�[�o�̓f�[�^
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

#define	KERNEL_MAX 16
cbuffer CbScene : register(b0)
{
    // �P�x�V�F�[�_�[
    float threshold; // ���P�x���o�̂��߂�臒l
    float intensity; // �u���[���̋��x
    float2 dummy;
    
    // GaussianBlur
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float2 textureSize;   // �򂷃e�N�X�`���̃T�C�Y
    float  kernelSize;    // �J�[�l���T�C�Y
    float  dummy1;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  StaticSampler(s0 ,\
                  filter = FILTER_MIN_MAG_MIP_LINEAR,\
                  addressU = TEXTURE_ADDRESS_CLAMP,\
                  addressV = TEXTURE_ADDRESS_CLAMP,\
                  addressW = TEXTURE_ADDRESS_CLAMP,\
                  mipLodBias = 0.0f,\
                  maxAnisotropy = 0,\
                  comparisonFunc  = COMPARISON_NEVER,\
                  borderColor = STATIC_BORDER_COLOR_TRANSPARENT_BLACK,\
                  minLOD = 0.0f,\
                  maxLOD = 3.402823466e+38f,\
                  space  =  0,\
                  visibility = SHADER_VISIBILITY_PIXEL\)"