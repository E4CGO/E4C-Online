//! @file FinalpassShader.h
//! @note 

#ifndef __GRAHICS_FINALPASS_SHADER_H__
#define __GRAHICS_FINALPASS_SHADER_H__

#include "SpriteShader.h"
#include "SpriteShaderDX12.h"

//***************************************************
// @class FinalpassShader
// @brief 最終合成
// @par   DX11
//***************************************************
class FinalpassShader : public SpriteShader
{
public:
	FinalpassShader(ID3D11Device* device);

	void Draw(const RenderContext& rc, const Sprite* sprite) override;
private:
	void UpdateConstantBuffer(const RenderContext& rc) override;
private:
	struct CBFinalpass
	{
		//	色調補正
		float	hueShift;	// 色相調整
		float	saturation;	// 彩度調整
		float	brightness;	// 明度調整
		float	dummy;
	};
};

//***************************************************
// @class FinalpassShaderDX12
// @brief 最終合成
// @par   DX12
//***************************************************
class FinalpassShaderDX12 : public SpriteShaderDX12
{
public:
	FinalpassShaderDX12(ID3D12Device* device);
	~FinalpassShaderDX12() override {};

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_FINALPASS_SHADER_H__