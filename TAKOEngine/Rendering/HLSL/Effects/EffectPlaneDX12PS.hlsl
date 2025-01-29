#include "../Primitives/PrimitiveDX12.hlsli"
//https://www.shadertoy.com/view/MdlXz8

Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

#define SHOW_TILING
#define EMISSION

#define TAU 6.28318530718
#define MAX_ITER 5

float4 main(VS_OUT pin) : SV_TARGET
{
    float time = timerGlobal * 0.5f + 23.0f;
    const float test_alpha = 0.6f;
    const float4 test_color = { 0.53f, 0.90f, 0.59f, 1.0f };
    const float test_light_factor = 26.0f;
    const float3 light_color = { 0.1f, 0.1f, 0.1f };
    
    // uv should be the 0-1 uv of texture...
    float2 uv = pin.texcoord.xy;
    
#ifdef SHOW_TILING
	float2 p = fmod(uv*TAU*2.0, TAU)-250.0;
#else
    float2 p = fmod(uv * TAU, TAU) - 250.0;
#endif
    float2 i = float2(p);
    float c = 1.0;
    float inten = .005;

    for (int n = 0; n < MAX_ITER; n++)
    {
        float t = time * (1.0 - (3.5 / float(n + 1)));
        i = p + float2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
        c += 1.0 / length(float2(p.x / (sin(i.x + t) / inten), p.y / (cos(i.y + t) / inten)));
    }
    c /= float(MAX_ITER);
    c = 1.17 - pow(c, 1.4);
    float3 colour = pow(abs(c), 8.0);
    colour = clamp(colour + float3(0.0, 0.35, 0.5), 0.0, 1.0);
    
    uv *= 2.0;
    
    float3 diffuse = 0;
    float3 specular = 0;
    
    float3 L = normalize(-directionalLightData.direction.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.tangent.xyz);
    float3 N = normalize(pin.normal.xyz);

    diffuse += light_color * colour.rgb * test_light_factor;
    specular += pow(max(0, dot(N, normalize(V + L))), 128);

    //float3 emissive = texture_map[2].Sample(sampler_states[LINEAR], pin.texcoord).rgb;
    //const float emissive_intensity = 3.0;
    
    return float4((diffuse + specular /* + emissive * emissive_intensity */), test_alpha) * test_color;

}