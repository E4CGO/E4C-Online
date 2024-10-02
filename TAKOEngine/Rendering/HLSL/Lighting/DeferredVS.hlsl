#include "Deferred.hlsli"
#include "../Model/Skinning.hlsli"

VS_OUT main( 
	float4 position : POSITION, 
	float4 color    : COLOR, 
	float2 texcoord : TEXCOORD)
{
	VS_OUT vout = (VS_OUT)0;

    vout.position = mul(viewProjection, position);
	vout.color    = color;
    vout.texcoord = texcoord;
	
    return vout;
}