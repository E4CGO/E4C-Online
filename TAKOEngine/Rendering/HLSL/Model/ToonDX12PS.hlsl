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
    
    // �}�e���A���萔
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(0.2f, 0.2f, 0.2f);
    float shiness = 128;
    
    //	����
    float3 A = ka.rgb * ambientLightColor.rgb;

	//	�g�U����
    float3 D;
	{
        float U = dot(-L, N) * 0.5f + 0.5f;
        D = ToonTex.Sample(sampler0, float2(U, 1)).rgb;

    }

	//	���ʔ���
    float3 S;
	{
        float U = dot(normalize(reflect(L, N)), E) * 0.5f + 0.5f;
        S = ToonTex.Sample(sampler0, float2(U, 1)).rgb;
        S = pow(saturate(S), 20) * 0.5f;
    }

	//	�������C�g
    float3 R;
	{
        float Rim = 1.0f - max(dot(N, E), 0.0f);
        float Ratio = max(dot(L, E), 0);
        R = ToonTex.Sample(sampler0, float2(Rim * Ratio, 1)).r;
    }
    
    // �_�����̏���
    float3 pointDiffuse  = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
		// ���C�g�x�N�g�����Z�o
        float3 lightVector = pin.position.xyz - pointLightData[i].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

		// ���C�g�̉e���͈͊O�Ȃ��̌v�Z�����Ȃ��B
        if (lightLength > pointLightData[i].range) continue;

		// �����������Z�o����
        float attenuate = clamp(1.0f - lightLength / pointLightData[i].range, 0.0, 1.0);
        lightVector     = lightVector / lightLength;
        pointDiffuse   += CalcLambertDiffuse(N, lightVector, pointLightData[i].color.rgb, kd.rgb) * attenuate;
        pointSpecular  += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }

	// �X�|�b�g���C�g�̏���
    float3 spotDiffuse  = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
		// ���C�g�x�N�g�����Z�o
        float3 lightVector = pin.position.xyz - spotLightData[i].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

        if (lightLength > spotLightData[i].range) continue;

		// �����������Z�o����
        float attenuate = clamp(1.0f - lightLength / spotLightData[i].range, 0.0, 1.0);

        lightVector = normalize(lightVector);

		// �p�x�������Z�o����attenuate�ɏ�Z����
        float3 spotDirection = spotLightData[i].direction;;
        float angle = dot(lightVector, spotDirection);
        float area  = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate  *= clamp(1.0f - (spotLightData[i].innerCorn - angle) / area, 0.0, 1.0);

        spotDiffuse  += CalcLambertDiffuse(N, lightVector, spotLightData[i].color.rgb, kd.rgb) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }
    
    C.rgb *= A + D + S + R;
    C.rgb += pointDiffuse + spotDiffuse + pointSpecular + spotSpecular;
    
    return C;
}