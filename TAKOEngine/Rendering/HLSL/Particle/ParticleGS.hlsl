#include "Particle.hlsli"

//最大出力頂点数
[maxvertexcount(4)]
void main(
	point GSInput input[1],
	inout TriangleStream<PSInput> output)
{
    PSInput ps;
    ps.Color = input[0].Color;
	
    //アニメーション
    int n = (int) (input[0].Param.y * 5 * 5); //25コマ
    float usize = (1.0 / 5); //1コマの横幅
    float vsize = (1.0 / 5); //1コマの縦幅
    float2 uv = float2((n % 5) * usize, (n / 5) * vsize);
    
    //カメラ空間に変換
    float4 ViewSpacePosition = mul(input[0].Position, view);
    float scale = input[0].Param.x;
        
	//左上
    float4 P = ViewSpacePosition + float4(-scale, scale, 0, 0);
    ps.Position = mul(P, Projection); //スクリーン空間
    ps.UV = uv;
    output.Append(ps); //1頂点出力
    
    //右上
    P = ViewSpacePosition + float4(scale, scale, 0, 0);
    ps.Position = mul(P, Projection);
    ps.UV = uv + float2(usize, 0);
    output.Append(ps);
    
    //左下
    P = ViewSpacePosition + float4(-scale, -scale, 0, 0);
    ps.Position = mul(P, Projection);
    ps.UV = uv + float2(0, vsize);
    output.Append(ps);
    
    //右下
    P = ViewSpacePosition + float4(scale, -scale, 0, 0);
    ps.Position = mul(P, Projection);
    ps.UV = uv + float2(usize, vsize);
    output.Append(ps);
}