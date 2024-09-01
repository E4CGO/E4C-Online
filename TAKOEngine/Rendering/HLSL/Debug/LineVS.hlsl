
#include "Line.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = mul(position, wvp);
	vout.color = color;

	return vout;
}
