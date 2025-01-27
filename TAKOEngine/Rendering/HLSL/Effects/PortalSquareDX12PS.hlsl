#include "PortalDX12.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color1 = { 0.473f, 0.004f, 0.714f, 1.0f };
    float2 factor1 = { 0.0f, 0.4f };
    float2 factor2 = { 0.3f, 1.0f };
    float2 factor3 = { 0.1f, 0.7f };
    float time_factor = 3.0f;
    float emission_factor = 3.0f;

    
    float t = timerGlobal * time_factor;
    float2 uv = pin.texcoord.xy - float2(0.5, 0.5); // オブジェクトの位置を計算して出力
    
    float2 st = float2(length(uv) * 2.0, atan2(uv.x, uv.y));
    
    st.y += st.x * 1.1;
        
    float x = fbm3d(float3((st.y), cos(st.y), pow(st.x, 0.3) + t * 0.1), 3);
    float y = fbm3d(float3(cos(1.0 - st.y), sin(1.0 - st.y), pow(st.x, 0.5) + t * 0.1), 4);
    float r = fbm3d(float3(x, y, st.x + t * 0.3), 5);
    
    r = fbm3d(float3(r - x, r - y, r + t * 0.3), 6);
    
    float c = (r + st.x * 3.0) / 6.0;
    float4 color = float4(smoothstep(factor1.x, factor1.y, c),
                          smoothstep(factor2.x, factor2.y, c),
                          smoothstep(factor3.x, factor3.y, c), 1.0);
    if (color.r <= 0.5 && color.g <= 0.5 && color.b <= 0.5)
        discard; //黒色は無視する 
    
    // モノクロにする
    color.rgb = (color.r + color.g + color.b) * 0.3333f;
    color.rgb *= color1;
#if 1
    return color;
#else
    float3 diffuse = 0;
    float3 specular = 0;
    
    float3 L = normalize(-light_direction.xyz);
    float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
    float3 N = normalize(pin.world_normal.xyz);

    diffuse += light_color * color.rgb * p_time_factor * emission_factor;
    specular += pow(max(0, dot(N, normalize(V + L))), 128);

    // EMISSION
    float3 emissive = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).rgb;
    float emissive_intensity = 3.0;
    

    return float4((diffuse + specular + emissive * emissive_intensity), 1.0);
#endif
}