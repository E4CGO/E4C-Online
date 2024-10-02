#include "Deferred.hlsli"

Texture2D<float4> albedoTexture   : register(t0); // アルベド
Texture2D<float4> normalTexture   : register(t1); // 法線
Texture2D<float4> worldPosTexture : register(t2); // ワールド座標

sampler Sampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// G-Bufferの内容を使ってライティング
    float4 albedo = albedoTexture.Sample(Sampler, pin.texcoord);
    float3 normal = normalTexture.Sample(Sampler, pin.texcoord).xyz;
    float4 worldPos = worldPosTexture.Sample(Sampler, pin.texcoord);
	
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);
    float3 k = float3(1, 1, 1);
    float shiness = 128;
    
    //環境光の計算
    float3 ambient = ambientLightColor.xyz * k;
    
    //ランバート拡散反射計算関数
    float3 directionalDiffuse  = CalcLambertDiffuse(normal, L, directionalLightData.color.rgb, k);
    float3 directionalSpecular = CalcPhongSpecular(normal, L, directionalLightData.color.rgb, E, shiness, k);
    
    //点光源の処理
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		//ライトベクトルを算出
        float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

		//ライトベクトルの長さを算出
        float lightLength = length(lightVector);

		//ライトの影響範囲外なら後の計算はしない
        if (lightLength >= pointLightData[i].range)
            continue;

		//距離減衰を算出
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

        lightVector = lightVector / lightLength;

		//ランバート拡散反射計算関数
        pointDiffuse += CalcLambertDiffuse(normal, lightVector, pointLightData[i].color.rgb, k.rgb) * attenuate;

		//フォン鏡面反射計算関数
        pointSpecular += CalcPhongSpecular(normal, lightVector, pointLightData[i].color.rgb, E, shiness, k.rgb) * attenuate;
    }
    
    //スポットライトの処理
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		//ライトベクトルを算出
        float3 lightVector = pin.position.xyz - spotLightData[i].position.xyz;

		//ライトベクトルの長さを算出
        float lightLength = length(lightVector);

        if (lightLength >= spotLightData[i].range)
            continue;

		//距離減衰を算出
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

        lightVector = normalize(lightVector);

		//角度減衰を算出してattenuateに乗算
        float3 spotDirection = normalize(spotLightData[i].direction.xyz);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

		//ランバート拡散反射計算関数
        spotDiffuse += CalcLambertDiffuse(normal, lightVector, spotLightData[i].color.rgb, k.rgb) * attenuate;

		//フォン鏡面反射計算関数
        spotSpecular += CalcPhongSpecular(normal, lightVector, spotLightData[i].color.rgb, E, shiness, k.rgb) * attenuate;
    }
    
    float4 color = albedo;
    color.rgb *= ambient + (directionalDiffuse + pointDiffuse + spotDiffuse);
    color.rgb += directionalSpecular + pointSpecular + spotSpecular;
    
	return color;
}