#include "Portal.hlsli"
#define LINEAR      1
SamplerState sampler_states[2] : register(s0);
Texture2D iChannel0 : register(t10);

// Ql
//https://www.shadertoy.com/view/WtGGRt  

float4 main(VS_OUT pin) : SV_TARGET
{
    // Normalized pixel coordinates (from 0 to 1)    
    const float xSpeed = 1.0;
    const float ySpeed = -1.0;
    
    float time = timerGlobal * 2.0f;
    
    // no floor makes it squiqqly
    float xCoord = pin.texcoord.x;
    float yCoord = pin.texcoord.y;
    
    float2 uv = float2(xCoord, yCoord);
    
    return iChannel0.Sample(sampler_states[LINEAR], uv) * pin.color;
}