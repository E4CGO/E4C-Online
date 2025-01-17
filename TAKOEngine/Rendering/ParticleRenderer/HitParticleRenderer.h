//! @file HitParticleRenderer.h
//! @note

#ifndef __GRAHICS_HIT_PARTICLE_RENDERER_H__
#define __GRAHICS_HIT_PARTICLE_RENDERER_H__

#include "TAKOEngine\Rendering\Shaders\HitParticleShader.h"
#include "TAKOEngine\Rendering\FrameBufferManager.h"

//***************************************************
// @class HitParticleRenderer
// @brief パーティクルの描画
// @par   [説明]
// @par   ヒットパーティクル
//***************************************************
class HitParticleRenderer
{
public:
	HitParticleRenderer(DirectX::XMFLOAT3 pos);
	~HitParticleRenderer();

	void Render(FrameBufferManager* framBuffer);

private:
	void CreateConstantBuffer();

	const Descriptor* UpdateSceneConstantBuffer(const RenderContextDX12& rc);

private:
	struct CBScene
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;

		DirectX::XMFLOAT4 emission_position;   // 放射位置
		float             time;                // 時間
		float             delta_time;          // 削除時間
		int               max_particle_count;  // パーティクルの最大数
		int               dummy;

		DirectX::XMUINT2 sprite_sheet_grid = { 3,2 };  // 画像タイルセット内の格子状のマス目
		float            respawn_radius = 1.0f;		   // リスポーン半径
	};

	Microsoft::WRL::ComPtr<ID3D12Resource>	d3d_cbv_resource;
	const Descriptor* cbv_descriptor = nullptr;
	CBScene* cbv_data = nullptr;

	std::unique_ptr<HitParticleCompute> m_compute;
};

#endif // !__GRAHICS_HIT_PARTICLE_RENDERER_H__
