#include "Toon.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D toonTex : register(t1); // �g�D�[���e�N�X�`��

SamplerState diffuseMapSamplerState : register(s0);

PSOutput main(VS_OUT pin)
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
    if (linearGamma != 1.0f) diffuseColor.rgb = pow(diffuseColor, 1.0f / linearGamma);
 
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);

	// �}�e���A���萔
    float3 ka = shaderData.toonShader.ka.xyz;
    float3 kd = shaderData.toonShader.kd.xyz;
    float3 ks = shaderData.toonShader.ks.xyz * 0.3;
    float shiness = shaderData.toonShader.shiness;
   
    // �����̌v�Z
    float3 ambient = ka * ambientLightColor.rgb;

	// ���s�����̃��C�e�B���O�v�Z
    float3 directionalDiffuse = CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks);
    //directionalDiffuse = clamp(directionalDiffuse * 0.5 + 0.5, 0, 1);
    
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
    directionalDiffuse  *= shadow;
    directionalSpecular *= shadow;
    
    // �_�����̏���
 //   float3 pointDiffuse = (float3) 0;
 //   float3 pointSpecular = (float3) 0;
 //   for (i = 0; i < pointLightCount; ++i)
 //   {
	//	// ���C�g�x�N�g�����Z�o
 //       float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

	//	// ���C�g�x�N�g���̒������Z�o
 //       float lightLength = length(lightVector);

	//	// ���C�g�̉e���͈͊O�Ȃ��̌v�Z�����Ȃ��B
 //       if (lightLength >= pointLightData[i].range) continue;

	//	// �����������Z�o����
 //       float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

 //       lightVector = lightVector / lightLength;
 //       pointDiffuse += CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, lightVector, pointLightData[i].color.rgb, kd.rgb) * attenuate;
 //       pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
 //   }
    
	//// �X�|�b�g���C�g�̏���
 //   float3 spotDiffuse = (float3) 0;
 //   float3 spotSpecular = (float3) 0;
 //   for (i = 0; i < spotLightCount; ++i)
 //   {
	//	// ���C�g�x�N�g�����Z�o
 //       float3 lightVector = pin.position.xyz - spotLightData[i].position.xyz;

	//	// ���C�g�x�N�g���̒������Z�o
 //       float lightLength = length(lightVector);

 //       if (lightLength >= spotLightData[i].range)
 //           continue;

	//	// �����������Z�o����
 //       float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

 //       lightVector = normalize(lightVector);

	//	// �p�x�������Z�o����attenuate�ɏ�Z����
 //       float3 spotDirection = normalize(spotLightData[i].direction.xyz);
 //       float angle = dot(spotDirection, lightVector);
 //       float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
 //       attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

 //       spotDiffuse += CalcToonDiffuse(toonTex, diffuseMapSamplerState, N, lightVector, spotLightData[i].color.rgb, kd.rgb) * attenuate;
 //       spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
 //   }
    
    float4 color = diffuseColor * float4(shadow, 1.0f); //float4(ambient, diffuseColor.a);
    //color.rgb += diffuseColor.rgb * (directionalDiffuse + pointDiffuse + spotDiffuse);
    //color.rgb += directionalSpecular + pointSpecular + spotSpecular;
    
    PSOutput output = (PSOutput) 0;
    output.Color      = color * materialColor;
    output.Normal.xyz = (N / 2.0f) + 0.5f;
    output.Normal.w   = 1.0f;
    output.Pos.xyz    = pin.position;
    output.Pos.w      = 1.0f;
    
    return output;
}