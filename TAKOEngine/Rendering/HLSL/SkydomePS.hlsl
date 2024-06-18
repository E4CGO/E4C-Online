#include "Skydome.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState linearSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return diffuseMap.Sample(linearSampler, pin.texcoord) * materialColor;
}