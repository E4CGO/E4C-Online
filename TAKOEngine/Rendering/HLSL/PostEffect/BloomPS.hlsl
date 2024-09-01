#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"

Texture2D colorMap : register(t0);
Texture2D luminanceMap : register(t1);
SamplerState linearSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	uint width, height;
	luminanceMap.GetDimensions(width, height);

	float4 color = colorMap.Sample(linearSampler, pin.texcoord);
	float alpha = color.a;

	float3 blurColor = 0;
	float gaussianKernelTotal = 0;

	const float PI = 3.14159265358979f;
	const int gaussianHalfKernelSize = 3;
	[unroll]
	for (int x = -gaussianHalfKernelSize; x <= +gaussianHalfKernelSize; x += 1)
	{
		[unroll]
		for (int y = -gaussianHalfKernelSize; y <= +gaussianHalfKernelSize; y += 1)
		{
			float gaussianKernel = exp(-(x * x + y * y) / (2.0 * gaussianSigma * gaussianSigma)) / (2 * PI * gaussianSigma * gaussianSigma);

			blurColor += luminanceMap.Sample(linearSampler, pin.texcoord + float2(x * 1.0 / width, y * 1.0 / height)).rgb * gaussianKernel;

			gaussianKernelTotal += gaussianKernel;
		}
	}
	blurColor /= gaussianKernelTotal;

	color.rgb += blurColor * bloomIntensity;

	const float exposure = 1.2;
	color.rgb = 1 - exp(-color.rgb * exposure);

	return color;
}