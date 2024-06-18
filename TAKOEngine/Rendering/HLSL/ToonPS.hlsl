#include "Toon.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);
Texture2D toonTex : register(t1); // トゥーンテクスチャ

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
    if (linearGamma != 1.0f) diffuseColor.rgb = pow(diffuseColor, 1.0f / linearGamma);
 
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);

	// マテリアル定数
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(1, 1, 1) * 0.3;
    float shiness = 128;
   
    // 環境光の計算
    float3 ambient = ka * ambientLightColor.rgb;

	// 平行光源のライティング計算
    float3 directionalDiffuse = CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks);
    //directionalDiffuse = clamp(directionalDiffuse * 0.5 + 0.5, 0, 1);
    
    // 点光源の処理
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    for (int i = 0; i < pointLightCount; ++i)
    {
		// ライトベクトルを算出
        float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

		// ライトベクトルの長さを算出
        float lightLength = length(lightVector);

		// ライトの影響範囲外なら後の計算をしない。
        if (lightLength >= pointLightData[i].range) continue;

		// 距離減衰を算出する
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

        lightVector = lightVector / lightLength;
        pointDiffuse += CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, lightVector, pointLightData[i].color.rgb, kd.rgb) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }
    

	// スポットライトの処理
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		// ライトベクトルを算出
        float3 lightVector = pin.position.xyz - spotLightData[i].position.xyz;

		// ライトベクトルの長さを算出
        float lightLength = length(lightVector);

        if (lightLength >= spotLightData[i].range)
            continue;

		// 距離減衰を算出する
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

        lightVector = normalize(lightVector);

		// 角度減衰を算出してattenuateに乗算する
        float3 spotDirection = normalize(spotLightData[i].direction.xyz);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

        spotDiffuse += CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, lightVector, spotLightData[i].color.rgb, kd.rgb) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }
    
    float4 color = float4(ambient, diffuseColor.a);
    color.rgb += diffuseColor.rgb * (directionalDiffuse + pointDiffuse + spotDiffuse);
    color.rgb += directionalSpecular + pointSpecular + spotSpecular;
    
    return color * materialColor;
}