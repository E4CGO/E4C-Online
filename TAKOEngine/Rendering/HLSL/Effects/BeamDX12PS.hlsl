#include "../Primitives/PrimitiveDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    const float3 light_color = { 0.1f, 0.1f, 0.1f };
    const float test_light_factor = 100.0f;
    float test_scale = -10.5f;
    float4 test_color = { 1.0f, 1.0f, 0.0f, 1.0f };
    
    const float xSpeed = 2.6f;
    const float ySpeed = -8.0f;
    
    const float3 test_color1 = float3(0.525f, 0.068f, 0.860);
    
    float time = timerGlobal * 0.1f;
    
    
    float xCoord = pin.texcoord.x + time * xSpeed;
    float yCoord = pin.texcoord.y + time * ySpeed;  
    
    float2 uv = float2(xCoord, yCoord);
    uv = uv * 2.0 - 1.0f;

    float4 col = texture0.Sample(sampler0, uv);

    float3 diffuse = 0;
    float3 specular = 0;

    diffuse += light_color * col.rgb * test_light_factor;
    
    float3 L = normalize(-directionalLightData.direction.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.tangent.xyz);
    float3 N = normalize(pin.normal.xyz);
    
    specular += pow(max(0, dot(N, normalize(V + L))), 128);

    col = float4((diffuse + specular /* + emissive * emissive_intensity*/), //emmision
            exp(pin.local_position.y * 2.1) /*gradation*/) * exp(pin.local_position.z * 0.0f * 10) * float4(test_color1.rgb, pin.color.w) /*color*/;
    if (col.r <= 0.8 && col.g <= 0.8 && col.b <= 0.8)
        discard;
    
    return col;
}