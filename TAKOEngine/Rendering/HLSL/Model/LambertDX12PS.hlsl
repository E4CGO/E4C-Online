#include "LambertDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D shadowMap : register(t1);

SamplerState sampler0 : register(s0);
SamplerState shadow_sampler : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = texture0.Sample(sampler0, pin.texcoord) * pin.color;
    
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);
    
    // マテリアル定数
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(1, 1, 1);
    float shiness = 128;
   
    // 環境光の計算
    float3 ambient = ka * ambientLightColor.xyz;
    
    // 平行光源の計算
    float3 directionalDiffuse  = ClacHalfLambert(N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks);
    
    // 平行光源の影なので、平行光源に対して影を適応
    float3 shadow = 1;
    {
        float3 shadowRexcoord = pin.shadow;
        
        //シャドウマップのUV範囲内か、深度値が範囲内か判定する
        if (shadowRexcoord.z >= 0 && shadowRexcoord.z <= 1 &&
			shadowRexcoord.x >= 0 && shadowRexcoord.x <= 1 &&
			shadowRexcoord.y >= 0 && shadowRexcoord.y <= 1)
        {
            //シャドウマップから深度値取得
            float depth = shadowMap.Sample(shadow_sampler, shadowRexcoord.xy).r;
           
            //深度値を比較して影かどうかを判定する
            if (shadowRexcoord.z - depth > shadowBias) //shadow = shadowColor;  
            {
                shadow = CalcShadowColorPCFFilter(shadowMap, shadow_sampler, shadowRexcoord, shadowColor, shadowBias);
            }
        }
    }
    directionalDiffuse *= shadow;
    directionalSpecular *= shadow;
    
    // 点光源の処理
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		// ライトベクトルを算出
        float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

		// ライトベクトルの長さを算出
        float lightLength = length(lightVector);

		// ライトの影響範囲外なら後の計算をしない。
        if (lightLength > pointLightData[i].range) continue;

		// 距離減衰を算出する
        float attenuate = clamp(1.0f - lightLength / pointLightData[i].range, 0.0, 1.0);
        lightVector = lightVector / lightLength;
        pointDiffuse += CalcLambertDiffuse(N, lightVector, pointLightData[i].color.rgb, kd.rgb) * attenuate;
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

        if (lightLength > spotLightData[i].range) continue;

		// 距離減衰を算出する
        float attenuate = clamp(1.0f - lightLength / spotLightData[i].range, 0.0, 1.0);

        lightVector = normalize(lightVector);

		// 角度減衰を算出してattenuateに乗算する
        float3 spotDirection = spotLightData[i].direction;;
        float angle = dot(lightVector, spotDirection);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= clamp(1.0f - (spotLightData[i].innerCorn - angle) / area, 0.0, 1.0);

        spotDiffuse += CalcLambertDiffuse(N, lightVector, spotLightData[i].color.rgb, kd.rgb) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }
    
    float4 color = diffuseColor;
    color.rgb *= ambient + (directionalDiffuse + pointDiffuse + spotDiffuse);
    color.rgb += directionalSpecular + pointSpecular + spotSpecular;
    
    return color;
}

