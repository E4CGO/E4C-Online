#include "Particle.hlsli"

Texture2D Animation : register(t0);
SamplerState sampler_repeat : register(s0);

float4 main(PSInput input) : SV_TARGET
{
    float4 tex = Animation.Sample(sampler_repeat, input.UV);
    tex.rgb *= tex.a;
    clip(tex.a - 0.1); // 透明度が0.1以下のピクセルは描画しない
    
    return tex;
    
    //中心からの距離
    //float2 d = input.UV - float2(0.5, 0.5);
    //float r = length(d) / 0.5; //r : 中心からの距離  0.0 ~ 0.5を0.0 ~ 1.0
    //r /= 0.5f;
    //float alpha = 1.0 - r; //中心 = 1.0
    
    //return input.Color * alpha;
}