#include "HitParticle.hlsli"
Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = pin.color;
    
	return color;
}