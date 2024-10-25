//! @file GaussianBlurShader.h
//! @note

#ifndef __GRAHICS_GAUSSIAN_BLUR_SHADER_H__
#define __GRAHICS_GAUSSIAN_BLUR_SHADER_H__

#include "SpriteShader.h"
#include "SpriteShaderDX12.h"

//*******************************************************
// @class GaussianBlurShader 
// @brief ブラー
// @par   DX11
//*******************************************************
class GaussianBlurShader : public SpriteShader
{
public:
	GaussianBlurShader(ID3D11Device* device);

private:
	struct CBFilter
	{
		DirectX::XMFLOAT4 weights[MaxKernelSize * MaxKernelSize];
		float kernelSize;
		DirectX::XMFLOAT2 texcel;
		float dummy;
	};
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
	// フィルター値計算
	void CalcGaussianFilter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData);
};

//*******************************************************
// @class GaussianBlurShader 
// @brief ブラー
// @par   DX12
//*******************************************************
class GaussianBlurShaderDX12 : public SpriteShaderDX12
{
public:
	GaussianBlurShaderDX12(ID3D12Device* device);
	~GaussianBlurShaderDX12() override {};

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_GAUSSIAN_BLUR_SHADER_H__