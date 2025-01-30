#include "Grass.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 degamma(float4 color)
{
    return float4(pow(color.rgb, 2.2), color.a);
}

[earlydepthstencil]
float4 main(GS_OUT pin) : SV_TARGET
{
    float4 base_color = degamma(texture0.Sample(sampler0, pin.texcoord));
    const float alpha = base_color.a;

    const float3 N = normalize(pin.normal.xyz);
    const float3 V = normalize(cameraPosition.xyz - pin.position.xyz);

    const float3 grass_withered_color = saturate(base_color.rgb + pin.color.rgb);
    float3 diffuse_color = lerp(grass_withered_color, grass_withered_color * 0.2, pin.texcoord.y);

    diffuse_color = lerp(
		0.75,
		diffuse_color,
		exp(-(1.2 - pin.texcoord.y) * 0));

    const float specular_intensity = exp(-pin.texcoord.y * 0.001);
    const float4 specular_color = grass_specular_color;
    const float shininess = 32.0;

    float3 radiance = 0.0;
    float3 diffuse = 0;
    float3 specular = 0;

    float3 L = 0;
    L = normalize(-directionalLightData.direction.xyz);
    diffuse = diffuse_color * max(0, dot(N, L) * 0.5 + 0.5);
    specular = pow(max(0, dot(N, normalize(V + L))), 32) * specular_intensity * specular_color.rgb * specular_color.w;
    radiance += (diffuse + specular) * directionalLightData.color.rgb * directionalLightData.color.w;
    
    float3 ambient = diffuse_color * 0.2;
    radiance += ambient;

    return float4(max(0, radiance), alpha);
}
