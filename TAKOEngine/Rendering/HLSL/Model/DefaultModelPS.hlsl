#include "DefaultModel.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
	
    //環境光の計算
    float3 ambient = shaderData.defaultShader.ka.rgb * ambientLightColor.xyz;
    
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    
    //ランバート拡散反射計算関数
    float3 directionalDiffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, shaderData.defaultShader.kd.rgb);
    
    // 平行光源の影なので、平行光源に対して影を適応
    float3 shadow = 1;
    for (int i = 0; i < ShadowmapCount; ++i)
    {
        float3 shadowRexcoord = pin.shadowTexcoord[i];
		
		//シャドウマップのUV範囲内か、深度値が範囲内か判定する
        if (shadowRexcoord.z >= 0 && shadowRexcoord.z <= 1 &&
			shadowRexcoord.x >= 0 && shadowRexcoord.x <= 1 &&
			shadowRexcoord.y >= 0 && shadowRexcoord.y <= 1)
        {
			//シャドウマップから深度値取得
            float depth = shadowMap[i].Sample(shadowSampler, shadowRexcoord.xy).r;

			//深度値を比較して影かどうかを判定する
            if (shadowRexcoord.z - depth > shadowBias[i]) //shadow = shadowColor;  
                shadow = CalcShadowColorPCFFilter(shadowMap[i], shadowSampler, shadowRexcoord, shadowColor, shadowBias[i]);
            break;
        }
    }
    directionalDiffuse *= shadow;
    
    float4 color = diffuseColor;
    color.rgb *= ambient + directionalDiffuse;
    
    return color;
}

