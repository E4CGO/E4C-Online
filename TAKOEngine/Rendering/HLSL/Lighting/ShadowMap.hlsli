#include "ShadowmapFunctions.hlsli"

//�V���h�E�}�b�v�̖���
static const int ShadowmapCount = 4;

Texture2D shadowMap[ShadowmapCount] : register(t10);
SamplerState shadowSampler : register(s10);

cbuffer CbShadowmap : register(b3)
{
	row_major float4x4 lightViewProjection[ShadowmapCount];  //���C�g�r���[�v���W�F�N�V�����s��
	float4             shadowBias;			                 //�[�x�l��r���̃I�t�Z�b�g
	float3             shadowColor;			                 //�e�̐F
    float              dummy;
}