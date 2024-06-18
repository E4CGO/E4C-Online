#include "GaussianBlurShader.h"

#include "TAKOEngine/Rendering/GpuResourceUtils.h"

GaussianBlurShader::GaussianBlurShader(ID3D11Device* device) : SpriteShader(device, "Data/Shader/SpriteVS.cso", "Data/Shader/GaussianBlurPS.cso")
{
	// シーン用バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CBFilter),
		constantBuffer.GetAddressOf()
	);
}

void GaussianBlurShader::UpdateConstantBuffer(const RenderContext& rc)
{
	//	ガウスフィルター値の計算
	CBFilter cbFilter;
	CalcGaussianFilter(cbFilter, rc.gaussianFilterData);
	rc.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbFilter, 0, 0);
}

void GaussianBlurShader::CalcGaussianFilter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData)
{
	int kernelSize = gaussianFilterData.kernelSize;
	// 偶数の場合は奇数に直す
	if (kernelSize % 2 == 0)	kernelSize++;
	kernelSize = max(1, min(MaxKernelSize - 1, kernelSize));
	cbFilter.kernelSize = static_cast<float>(kernelSize);
	cbFilter.texcel.x = 1.0f / gaussianFilterData.textureSize.x;
	cbFilter.texcel.y = 1.0f / gaussianFilterData.textureSize.y;

	float deviationPow2 = 2.0f * gaussianFilterData.deviation * gaussianFilterData.deviation;
	float sum = 0.0f;
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
		{
			cbFilter.weights[id].x = (float)x;
			cbFilter.weights[id].y = (float)y;
			cbFilter.weights[id].z = (1 / DirectX::XM_PI * deviationPow2 * exp(0 - (x * x + y * y) / deviationPow2));

			sum += cbFilter.weights[id].z;
			id++;
		}
	}
	for (int i = 0; i < cbFilter.kernelSize * cbFilter.kernelSize; i++)
	{
		cbFilter.weights[i].z /= sum;
	}
}