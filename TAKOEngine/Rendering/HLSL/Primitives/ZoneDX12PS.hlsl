#include "../Model/PhongDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D normalMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

#define TAU 6.28318530718
#define MAX_ITER 5

float4 main(VS_OUT pin) : SV_TARGET
{
    const float3 light_color = { 0.3f, 0.3f, 0.3f };
    const float test_light_factor = 5.0f;
    
    float time = timerGlobal * 1.1;
    //float time = test_TIME * .5 + 23.0;
    // uv should be the 0-1 uv of texture...
    float2 uv = pin.texcoord.xy;
    
#ifdef SHOW_TILING
	vec2 p = mod(uv*TAU*2.0, TAU)-250.0;
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

#ifdef SHOW_TILING
	// Flash tile borders...
	vec2 pixel = 2.0 / iResolution.xy;
	uv *= 2.0;
	float f = floor(mod(iTime*.5, 2.0)); 	// Flash value.
	vec2 first = step(pixel, uv) * f;		   	// Rule out first screen pixels and flash.
	uv  = step(fract(uv), pixel);				// Add one line of pixels per tile.
	colour = mix(colour, vec3(1.0, 1.0, 0.0), (uv.x + uv.y) * first.x * first.y); // Yellow line
#endif
    
    float4 fragColor = float4(colour, 0.5);
    
    // ブルームが必要な場合  (static_mesh_ps.hlsl より　抜粋)

    float3 diffuse = 0;
    float3 specular = 0;
    
    //float3 L = normalize(-light_direction.xyz);
    //float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    //float3 N = normalize(pin.world_normal.xyz);

    diffuse += light_color * fragColor.rgb * test_light_factor;
    //specular += pow(max(0, dot(N, normalize(V + L))), 128);

    // EMISSION
    float4 emissive = texture0.Sample(sampler0, pin.texcoord);
    const float emissive_intensity = 0.1;

    return float4((diffuse /* + specular +  emissive * emissive_intensity */), fragColor.a) * pin.color;
    //fragColor.a = 0.5f;
    return fragColor;
}