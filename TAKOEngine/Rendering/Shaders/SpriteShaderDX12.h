//! @file SpriteShaderDX12.h
//! @note 

#ifndef __GRAHICS_SPRITE_SHADER_DX12_H__
#define __GRAHICS_SPRITE_SHADER_DX12_H__

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"

//****************************************************
// @class SpriteShaderDX12
// @brief スプライトシェーダーの基底クラス(DX12)
// @par   
//****************************************************
class SpriteShaderDX12
{
protected:
	//コンストラクタ
	SpriteShaderDX12() {};

public:
	//デストラクタ
	virtual ~SpriteShaderDX12() = default;

	// 描画
	virtual void Render(const RenderContextDX12& rc, SpriteDX12* sprite) = 0;
};

//****************************************************
// @class DefaultSpriteShaderDX12
// @brief DX12の描画テスト
// @par   テクスチャをそのまま描画する
//****************************************************
class DefaultSpriteShaderDX12 : public SpriteShaderDX12
{
public:
	DefaultSpriteShaderDX12(ID3D12Device* device);
	~DefaultSpriteShaderDX12() override;

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

//****************************************************
// @class ComputeShader
// @brief コンピュートシェーダーの基底クラス
// @par   
//****************************************************
class ComputeShader
{
protected:
	int MaxParticles = 0;

public:
	ComputeShader() {};
	virtual ~ComputeShader() = default;

	// 計算
	virtual void Compute(const RenderContextDX12& rc) = 0;
	virtual void Compute(const RenderContextDX12& rc, SpriteDX12* sprite) = 0;

	// 最大パーティクル数取得
	int GetMaxParticle() { return MaxParticles; }

public:
	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vbv_uav_resource;
	D3D12_VERTEX_BUFFER_VIEW				d3d_vbv = {};
	const Descriptor* uav_descriptor = nullptr;
	const Descriptor* srv_descriptor = nullptr;
};

#endif // !__GRAHICS_SPRITE_SHADER_DX12_H__
