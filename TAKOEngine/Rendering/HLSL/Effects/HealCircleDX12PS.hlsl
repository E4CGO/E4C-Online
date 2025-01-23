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
    float4 test_color = { 0.0f, 0.3f, 0.1f, 1.0f };
    
    float time = timerGlobal * 0.6;
    const float ySpeed = -1.0;
    const float xSpeed = 0.0;
    const float loop_speed = 2.0f;
    
    // no floor makes it squiqqly
    float xCoord = pin.texcoord.x + time * xSpeed;
    float yCoord = pin.texcoord.y + time * -ySpeed;
    
    float2 uv = float2(xCoord, yCoord);
    // V座標（Y成分）を反転 
    
    float4 col = texture0.Sample(sampler0, uv);
    
    float3 diffuse = 0;
   
    diffuse += light_color * col.rgb * test_light_factor;
    col = float4((diffuse /* + specular + emissive * emissive_intensity*/), //emmision
            exp(pin.local_position.y * test_scale) /*gradation*/) * test_color /*color*/;

    return col;
}