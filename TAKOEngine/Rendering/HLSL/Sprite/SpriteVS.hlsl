#include "Sprite.hlsli"

// 頂点シェーダーエントリポイント
[RootSignature(ROOT_SIG)]
VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;

	vout.position = position;
	vout.color = color;
	vout.texcoord = texcoord;

    return vout;
}