#include "ToonDX12.hlsli"

[maxvertexcount(6)]
void main( triangle VS_OUT input[3], inout TriangleStream<VS_OUT> output)
{
	VS_OUT data = (VS_OUT) 0;
    int i;
	
	//通常の面を作成
	for ( i = 0; i < 3; ++i)
	{
        data.vertex   = input[i].vertex;
        data.normal   = input[i].normal;
        data.texcoord = input[i].texcoord;
        data.position = input[i].position;
        data.tangent  = input[i].tangent;
        data.binormal = input[i].binormal;
        data.color    = input[i].color;
      
        output.Append(data);
    }
    
    //	追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
    output.RestartStrip();

	//	裏面で拡大して出力を作成
    for (i = 2; i >= 0; --i)
    {
        float4x4 viewProjection = mul(view, Projection);
        data.vertex = mul(float4(input[i].position.xyz + input[i].normal * 0.02f, 1), mul(world_transform, viewProjection));
        data.normal   = input[i].normal;
        data.texcoord = input[i].texcoord;
        data.position = input[i].position;
        data.tangent  = input[i].tangent;
        data.binormal = input[i].binormal;
        data.color    = float4(0, 0, 0, 1);
        
        output.Append(data);
    }

    //	追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
    output.RestartStrip();
}