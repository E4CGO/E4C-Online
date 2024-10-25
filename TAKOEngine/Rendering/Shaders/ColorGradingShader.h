//! @file ColorGradingShader
//! @note

#ifndef __GRAHICS_COLOR_GRADING_SHADER_H__
#define __GRAHICS_COLOR_GRADING_SHADER_H__

#include "SpriteShader.h"
#include "SpriteShaderDX12.h"

//***************************************************
// @class ColorGradingShader
// @brief 色調補正
// @par   DX11
//***************************************************
class ColorGradingShader : public SpriteShader
{
public:
	ColorGradingShader(ID3D11Device* device);
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CBColorGrading
	{
		float	hueShift;	// 色相調整
		float	saturation;	// 彩度調整
		float	brightness;	// 明度調整
		float	dummy;
	};
};

//***************************************************
// @class ColorGradingShaderDX12
// @brief 色調補正
// @par   DX12
//***************************************************
class ColorGradingShaderDX12 : public SpriteShaderDX12
{
public:
	ColorGradingShaderDX12(ID3D12Device* device);
	~ColorGradingShaderDX12() override {};

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_COLOR_GRADING_SHADER_H__