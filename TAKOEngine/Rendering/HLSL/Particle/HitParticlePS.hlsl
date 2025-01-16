#include "HitParticle.hlsli"

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = pin.color;
    
	return color;
}