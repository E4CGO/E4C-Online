// 頂点シェーダー出力データ
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
//コンスタントバッファ登録
cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
};
