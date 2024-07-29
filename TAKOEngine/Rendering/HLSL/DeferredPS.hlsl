#include "Deferred.hlsli"

Texture2D<float4> albedoTexture   : register(t0); // �A���x�h
Texture2D<float4> normalTexture   : register(t1); // �@��
Texture2D<float4> worldPosTexture : register(t2); // ���[���h���W

sampler Sampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// G-Buffer�̓��e���g���ă��C�e�B���O
    float4 albedo = albedoTexture.Sample(Sampler, pin.texcoord);
    float3 normal = normalTexture.Sample(Sampler, pin.texcoord).xyz;
    float4 worldPos = worldPosTexture.Sample(Sampler, pin.texcoord);
	
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = normalize(cameraPosition.xyz - pin.position.xyz);
    float3 k = float3(1, 1, 1);
    float shiness = 128;
    
    //�����̌v�Z
    float3 ambient = ambientLightColor.xyz * k;
    
    //�����o�[�g�g�U���ˌv�Z�֐�
    float3 directionalDiffuse  = CalcLambertDiffuse(normal, L, directionalLightData.color.rgb, k);
    float3 directionalSpecular = CalcPhongSpecular(normal, L, directionalLightData.color.rgb, E, shiness, k);
    
    //�_�����̏���
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		//���C�g�x�N�g�����Z�o
        float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

		//���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

		//���C�g�̉e���͈͊O�Ȃ��̌v�Z�͂��Ȃ�
        if (lightLength >= pointLightData[i].range)
            continue;

		//�����������Z�o
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

        lightVector = lightVector / lightLength;

		//�����o�[�g�g�U���ˌv�Z�֐�
        pointDiffuse += CalcLambertDiffuse(normal, lightVector, pointLightData[i].color.rgb, k.rgb) * attenuate;

		//�t�H�����ʔ��ˌv�Z�֐�
        pointSpecular += CalcPhongSpecular(normal, lightVector, pointLightData[i].color.rgb, E, shiness, k.rgb) * attenuate;
    }
    
    //�X�|�b�g���C�g�̏���
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		//���C�g�x�N�g�����Z�o
        float3 lightVector = pin.position.xyz - spotLightData[i].position.xyz;

		//���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

        if (lightLength >= spotLightData[i].range)
            continue;

		//�����������Z�o
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

        lightVector = normalize(lightVector);

		//�p�x�������Z�o����attenuate�ɏ�Z
        float3 spotDirection = normalize(spotLightData[i].direction.xyz);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

		//�����o�[�g�g�U���ˌv�Z�֐�
        spotDiffuse += CalcLambertDiffuse(normal, lightVector, spotLightData[i].color.rgb, k.rgb) * attenuate;

		//�t�H�����ʔ��ˌv�Z�֐�
        spotSpecular += CalcPhongSpecular(normal, lightVector, spotLightData[i].color.rgb, E, shiness, k.rgb) * attenuate;
    }
    
    float4 color = albedo;
    color.rgb *= ambient + (directionalDiffuse + pointDiffuse + spotDiffuse);
    color.rgb += directionalSpecular + pointSpecular + spotSpecular;
    
	return color;
}