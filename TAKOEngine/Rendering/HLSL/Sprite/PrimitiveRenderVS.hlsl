#include "PrimitiveRender.hlsli"
// ���_�V�F�[�_�[�G���g���|�C���g
VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    vout.position = mul(position, viewProjection);
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}