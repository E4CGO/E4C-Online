//! @file LuminanceExtractionShader.h
//! @note 

#ifndef __GRAHICS_LUMINANCE_EXTRACTION_SHADER_H__
#define __GRAHICS_LUMINANCE_EXTRACTION_SHADER_H__

#include "SpriteShader.h"
#include "SpriteShaderDX12.h"

//********************************************************
// @class LuminanceExtractionShader
// @brief 輝度シェーダー
// @par   DX11
//********************************************************
class LuminanceExtractionShader : public SpriteShader
{
public:
	LuminanceExtractionShader(ID3D11Device* device);
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CBLuminanceExtraction
	{
		float				threshold;	// 閾値
		float				intensity;	// ブルームの強度
		DirectX::XMFLOAT2	dummy2;
	};
};

//********************************************************
// @class LuminanceExtractionShaderDX12
// @brief 輝度シェーダー
// @par   DX12
//********************************************************
class LuminanceExtractionShaderDX12 : public SpriteShaderDX12
{
public:
	LuminanceExtractionShaderDX12(ID3D12Device* device);
	~LuminanceExtractionShaderDX12() override {};

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_LUMINANCE_EXTRACTION_SHADER_H__