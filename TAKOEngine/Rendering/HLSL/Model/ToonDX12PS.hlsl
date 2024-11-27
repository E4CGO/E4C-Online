#include "ToonDX12.hlsli"

Texture2D texture0 : register(t0);
Texture2D ToonTex  : register(t1);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 C = texture0.Sample(sampler0, pin.texcoord) * pin.color;
    float3 N = normalize(pin.normal);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);
    float3 L = normalize(directionalLightData.direction.xyz);
    
    // マテリアル定数
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(0.2f, 0.2f, 0.2f);
    float shiness = 20;
    
    //	環境光
    float3 A = ka.rgb * ambientLightColor.rgb;

	//	拡散反射
    float3 D;
	{
        float toonScale = 0.5f; // トーンを調整する係数
        float U = dot(-L, N) * 0.5f + 0.5f;
        D = ToonTex.Sample(sampler0, float2(U, 1)).rgb * toonScale;
    }

	//	鏡面反射
    float3 S;
	{
        float U = dot(normalize(reflect(L, N)), E) * 0.5f + 0.5f;
        S = ToonTex.Sample(sampler0, float2(U, 1)).rgb;
        S = pow(saturate(S), 20) * 0.2f;
    }

	//	リムライト
    float3 R;
	{
        float Rim = 1.0f - max(dot(N, E), 0.0f);
        float Ratio = max(dot(L, E), 0);
        R = ToonTex.Sample(sampler0, float2(Rim * Ratio, 1)).r * 0.4f;
    }
    
    // 点光源の処理
    float3 pointDiffuse  = (float3) 0;
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
        lightVector     = lightVector / lightLength;
        pointDiffuse   += CalcLambertDiffuse(N, lightVector, pointLightData[i].color.rgb, kd.rgb) * attenuate;
        pointSpecular  += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }

	// スポットライトの処理
    float3 spotDiffuse  = (float3) 0;
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
        float area  = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate  *= clamp(1.0f - (spotLightData[i].innerCorn - angle) / area, 0.0, 1.0);

        spotDiffuse  += CalcLambertDiffuse(N, lightVector, spotLightData[i].color.rgb, kd.rgb) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }
    
    C.rgb *= A + (D + pointDiffuse + spotDiffuse);
    C.rgb += S + pointSpecular + spotSpecular;
    
    return C;
}