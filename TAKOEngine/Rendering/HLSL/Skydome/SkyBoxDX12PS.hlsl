#include "SkyBoxDX12.hlsli"

TextureCube skymap : register(t0);

SamplerState sampler0 : register(s0);

float4 degamma(float4 color)
{
    return float4(pow(color.rgb, 2.2f), color.a);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 adjustedPosition = float3(pin.position.x, pin.position.z, -pin.position.y);
    //float4 diffuseColor = degamma(skymap.Sample(sampler0, adjustedPosition));
    float4 diffuseColor = (skymap.Sample(sampler0, adjustedPosition));
        
    float3 reflection = diffuseColor.rgb * materialColor.rgb;
    float alpha = diffuseColor.a * materialColor.a;
    
    const float intensity = 0.8;
    
    return float4(reflection.rgb * intensity * directionalLightData.color.rgb * directionalLightData.color.w, alpha);
}