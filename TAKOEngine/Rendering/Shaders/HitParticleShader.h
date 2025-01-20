//! @file HitParticleShader.h
//! @note

#ifndef __GRAHICS_HIT_PARTICLE_SHADER_H__
#define __GRAHICS_HIT_PARTICLE_SHADER_H__

#include "SpriteShaderDX12.h"

//***************************************************
// @class HitParticleCompute
// @brief パーティクルの計算
// @par   [説明]
// @par   コンピュートシェーダーで計算を行う
//***************************************************
class HitParticleCompute
{
protected:
	int MaxParticles = 0;

private:
	//パーティクル頂点構造体
	struct Point
	{
		int state = 1; // 1:alive, 0:dead

		DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT3 position = { 0, 0, 0 };
		DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
		float size = 0;

		float age = 0;		 //年齢
		float lifespan = 0;  //寿命

		int chip = 0;
	};
	std::vector<Point> points;

public:
	HitParticleCompute(ID3D12Device* device, DirectX::XMFLOAT3 pos);
	~HitParticleCompute();

	//計算
	void Compute(const RenderContextDX12& rc);

	DirectX::XMFLOAT3 GetPosition() { return m_position; }

	// 最大パーティクル数取得
	int GetMaxParticle() { return MaxParticles; }

public:
	const Descriptor* uav_descriptor = nullptr;
	const Descriptor* srv_descriptor = nullptr;
	D3D12_VERTEX_BUFFER_VIEW d3d_vbv = {};

private:
	// フレームリソース生成
	void CreateFrameResource();

	// frac関数（小数部分を返す）
	float frac(float x) 
	{
		return x - std::floor(x);
	}

	float rand(float n)
	{
		// 結果は0以上1未満の値
		return frac(std::sin(n) * 43758.5453123f);
	}

	UINT align(UINT num, UINT alignment)
	{
		return (num + (alignment - 1)) & ~(alignment - 1);
	}

	DirectX::XMFLOAT3 m_position = {0,0,0};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_d3d_root_signature;

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_vbv_uav_resource;
};

//***************************************************
// @class HitParticleShader
// @brief パーティクルのシェーダー
// @par   [説明]
// @par   ヒットエフェクト
//***************************************************
class HitParticleShader : public SpriteShaderDX12
{
public:
	HitParticleShader(ID3D12Device* device);
	~HitParticleShader() override;

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_HIT_PARTICLE_SHADER_H__
