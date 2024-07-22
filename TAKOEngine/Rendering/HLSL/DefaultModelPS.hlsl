#include "DefaultModel.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
	
    //�����̌v�Z
    float3 ambient = shaderData.defaultShader.ka.rgb * ambientLightColor.xyz;
    
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    
    //�����o�[�g�g�U���ˌv�Z�֐�
    float3 directionalDiffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, shaderData.defaultShader.kd.rgb);
    
    // ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
    float3 shadow = 1;
    for (int i = 0; i < ShadowmapCount; ++i)
    {
        float3 shadowRexcoord = pin.shadowTexcoord[i];
		
		//�V���h�E�}�b�v��UV�͈͓����A�[�x�l���͈͓������肷��
        if (shadowRexcoord.z >= 0 && shadowRexcoord.z <= 1 &&
			shadowRexcoord.x >= 0 && shadowRexcoord.x <= 1 &&
			shadowRexcoord.y >= 0 && shadowRexcoord.y <= 1)
        {
			//�V���h�E�}�b�v����[�x�l�擾
            float depth = shadowMap[i].Sample(shadowSampler, shadowRexcoord.xy).r;

			//�[�x�l���r���ĉe���ǂ����𔻒肷��
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

