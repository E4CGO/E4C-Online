//! @file ParticleRenderer.h
//! @note

#ifndef __GRAHICS_PARTICLE_RENDERER_H__
#define __GRAHICS_PARTICLE_RENDERER_H__

#include "TAKOEngine\Rendering\Shaders\ParticleShader.h"

//***************************************************
// @class InjectionParticleRenderer
// @brief パーティクルの描画
// @par   [説明]
// @par   噴射パーティクル
//***************************************************
class InjectionParticleRenderer
{
public:
	InjectionParticleRenderer();
	~InjectionParticleRenderer();

	void Render(FrameBufferManager* framBuffer);

private:
	void CreateConstantBuffer();

	const Descriptor* UpdateSceneConstantBuffer(const RenderContextDX12& rc);

private:
	enum class ParticleSprite
	{
		Noise,
		Anime,

		EnumCount
	};

	struct CBScene
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;

		// パーティクル情報
		float elapsedTime; //経過時間
		float deltaTime;   //フレーム経過時間
		int emitCount;     //出現させる個数
		int emitIndex;     //現在の出現待ち

		DirectX::XMFLOAT4 startColor;
		DirectX::XMFLOAT4 endColor;

		float scale;
		float lifetime;

		//ノイズ
		float noiseSpeed;
		float noisePower;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
	const Descriptor* cbv_descriptor = nullptr;
	CBScene* cbv_data = nullptr;

	// 描画用スプライト
	std::unique_ptr<SpriteDX12> 	m_sprites[static_cast<int>(ParticleSprite::EnumCount)];

	float m_emitRate; //1秒あたりの個数
	float m_emitWork;
};

#endif // !__GRAHICS_PARTICLE_RENDERER_H__
