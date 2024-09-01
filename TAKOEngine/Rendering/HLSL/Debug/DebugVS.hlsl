
#include "Debug.hlsli"

VS_OUT main(float4 position     : POSITION)
{
	VS_OUT vout;
	vout.position = mul(position, worldViewProjection);
	vout.color = color;

	return vout;
}
