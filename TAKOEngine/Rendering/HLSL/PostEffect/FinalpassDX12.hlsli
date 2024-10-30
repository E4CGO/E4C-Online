﻿// 頂点シェーダー出力データ
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

#define	KERNEL_MAX 16
cbuffer CbScene : register(b0)
{
    // 輝度シェーダー
    float threshold; // 高輝度抽出のための閾値
    float intensity; // ブルームの強度
    float2 dummy;
    
    // GaussianBlur
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float2 textureSize;   // 暈すテクスチャのサイズ
    float  kernelSize;    // カーネルサイズ
    float  dummy1;
    
    // ColorGrading
    float hueShift;	    // 色相調整
	float saturation;	// 彩度調整
	float brightness;	// 明度調整
	float dummy2;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_PIXEL), \
                  DescriptorTable(SRV(t1), visibility=SHADER_VISIBILITY_PIXEL), \
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
