#include "Sprite.hlsli"

#define	KERNEL_MAX 16
cbuffer CBFilter : register(b0)
{
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float kernelSize;
    float2 texcel;
    float dummy;
};
