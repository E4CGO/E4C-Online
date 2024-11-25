#include "ToonDX12.hlsli"

[maxvertexcount(6)]
void main( triangle VS_OUT input[3], inout TriangleStream<VS_OUT> output)
{
	VS_OUT data = (VS_OUT) 0;
    int i;
	
	//�ʏ�̖ʂ��쐬
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
    
    //	�ǉ��������_�Ńv���~�e�B�u���\�����A�V�����v���~�e�B�u�𐶐�����
    output.RestartStrip();

	//	���ʂŊg�債�ďo�͂��쐬
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

    //	�ǉ��������_�Ńv���~�e�B�u���\�����A�V�����v���~�e�B�u�𐶐�����
    output.RestartStrip();
}