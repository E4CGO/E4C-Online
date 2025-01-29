#include "../Primitives/PrimitiveDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    const float3 light_color = { 0.6f, 0.6f, 0.6f };
    const float test_light_factor = 200.0f;
    float test_scale = -5.5f;
    float4 test_color = { 0.0f, 0.4f, 0.6f, 1.0f };
    
    float time = timerGlobal * 0.1f;
    
    const float xSpeed = 1.0;
    const float ySpeed = -1.0;
    
    // no floor makes it squiqqly
    float xCoord = pin.texcoord.x + time * xSpeed;
    float yCoord = pin.texcoord.y + time * ySpeed;
    
    float2 uv = float2(xCoord, yCoord);
    
    float4 col = texture0.Sample(sampler0, uv);
    
    float3 diffuse = 0;
    float3 specular = 0;
    
    float3 L = normalize(-directionalLightData.direction.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.tangent.xyz);
    float3 N = normalize(pin.normal.xyz);
    
    diffuse += light_color * col.rgb * test_light_factor;
    specular += pow(max(0, dot(N, normalize(V + L))), 128);
   
    col = float4((diffuse  + specular /* + emissive * emissive_intensity*/), //emmision
            exp(pin.local_position.y * test_scale) /*gradation*/) * test_color /*color*/;

    return col;
}