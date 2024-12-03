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
class ParticleCompute
{
private:
	int MaxParticles = 0;  //最大パーティクル数
	
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
	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vbv_uav_resource;
	D3D12_VERTEX_BUFFER_VIEW				d3d_vbv;
	const Descriptor* uav_descriptor = nullptr;

	ParticleCompute(ID3D12Device* device);
	~ParticleCompute();

	//計算
	void Compute(const RenderContextDX12& rc, SpriteDX12* sprite);

	// 最大パーティクル数取得
	int GetMaxParticle() { return MaxParticles; }

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
