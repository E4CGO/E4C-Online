cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    
    float ElapsedTime; // 経過時間
    float DeltaTime;   // フレーム経過時間
    int EmitCount;     // 出現させる個数
    int EmitIndex;     // 現在の出現待ちインデックス
    float4 StartColor;
    float4 EndColor;
    
    float Scale;
    float Lifetime;
    float NoiseSpeed;
    float NoisePower;
};

struct VSInput
{
    float4 Position : POSITION;
    float3 Param : PARAM;
    float4 Color : COLOR;
};

struct GSInput
{
    float4 Position : POSITION;
    float3 Param : PARAM;
    float4 Color : COLOR;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD0;
    float4 Color    : COLOR;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_ALL), \
                  StaticSampler(s0 ,\
                  filter = FILTER_MIN_MAG_MIP_LINEAR,\
                  addressU = TEXTURE_ADDRESS_WRAP,\
                  addressV = TEXTURE_ADDRESS_WRAP,\
                  addressW = TEXTURE_ADDRESS_WRAP,\
                  mipLodBias = 0.0f,\
                  maxAnisotropy = 1,\
                  comparisonFunc  = COMPARISON_NEVER,\
                  borderColor = STATIC_BORDER_COLOR_TRANSPARENT_BLACK,\
                  minLOD = 0.0f,\
                  maxLOD = 3.402823466e+38f,\
                  space  =  0,\
                  visibility = SHADER_VISIBILITY_PIXEL)"
