// ���_�V�F�[�_�[�o�̓f�[�^
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
//�R���X�^���g�o�b�t�@�o�^
cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
};
