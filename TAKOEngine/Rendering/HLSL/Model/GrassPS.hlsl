#include "Grass.hlsli"

//Texture2D heightmap : register(t0);
//Texture2D colormap : register(t1);
//Texture2D normalmap : register(t2);

float4 main(GS_OUT pin) : SV_TARGET
{
    float3 L = -directionalLightData.direction.xyz;
    float3 N = normalize(pin.normal.xyz);

    float diffuse_factor = max(0, dot(N, L) * 0.5 + 0.5);

    const float3 tip_color = float3(0, 1, 0);
    const float3 root_color = float3(0.2, 0.1, 0);
    return float4(lerp(tip_color, root_color, pin.texcoord.y) * diffuse_factor, pin.texcoord.y);
}
