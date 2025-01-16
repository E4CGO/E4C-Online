//! @file ParticleShader.h
//! @note

#ifndef __GRAHICS_PARTICAL_SHADER_H__
#define __GRAHICS_PARTICAL_SHADER_H__

#include "SpriteShaderDX12.h"

//***************************************************
// @class ParticleCompute
// @brief パーティクルの計算
// @par   [説明]
// @par   コンピュートシェーダーで計算を行う
//***************************************************
class ParticleCompute : public ComputeShader
{
private:
	//パーティクル頂点構造体
	struct Point
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 param;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 velocity;
		float Life;
		float LifeTime;  //寿命
	};
	std::vector<Point> points;

public:
	ParticleCompute(ID3D12Device* device);
	~ParticleCompute() override;

	//計算
	void Compute(const RenderContextDX12& rc) override {};
	void Compute(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	// フレームリソース生成
	void CreateFrameResource();

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_d3d_root_signature;
};

//***************************************************
// @class ParticleShader
// @brief パーティクルのシェーダー
// @par   [説明]
//***************************************************
class ParticleShader : public SpriteShaderDX12
{
public:
	ParticleShader(ID3D12Device* device);
	~ParticleShader() override;

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_PARTICAL_SHADER_H__
