#include "GaussianBlur.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = (float4) 0;
    color.a = 1;
    for (int i = 0; i < kernelSize * kernelSize; i++)
    {
        float2 offset = texcel * weights[i].xy;
        float weight = weights[i].z;
        color.rgb += texture0.Sample(sampler0, pin.texcoord + offset).rgb * weight;
    }
    
    return color * pin.color;
}
