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
    row_major float4x4 world_transform;
    row_major float4x4 boneTransforms[255];
};

cbuffer CbMaterial : register(b2)
{
    float4 materialColor;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_VERTEX), \
                  DescriptorTable(CBV(b1), visibility=SHADER_VISIBILITY_VERTEX), \
                  DescriptorTable(CBV(b2), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(Sampler(s0), visibility=SHADER_VISIBILITY_PIXEL)"

//#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
//                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_VERTEX), \
//                  DescriptorTable(CBV(b1), visibility=SHADER_VISIBILITY_VERTEX), \
//                  DescriptorTable(CBV(b2), visibility=SHADER_VISIBILITY_ALL), \
//                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
//                  StaticSampler(s0 ,\
//                  filter = FILTER_MIN_MAG_MIP_LINEAR,\
//                  addressU = TEXTURE_ADDRESS_WRAP,\
//                  addressV = TEXTURE_ADDRESS_WRAP,\
//                  addressW = TEXTURE_ADDRESS_WRAP,\
//                  mipLodBias = 0.0f,\
//                  maxAnisotropy = 16,\
//                  comparisonFunc  = COMPARISON_NEVER,\
//                  borderColor = STATIC_BORDER_COLOR_TRANSPARENT_BLACK,\
//                  minLOD = 0.0f,\
//                  maxLOD = 3.402823466e+38f,\
//                  space  =  0,\
//                  visibility = SHADER_VISIBILITY_ALL\)"