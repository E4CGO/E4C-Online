//! @file ParticleObject.h
//! @note

#ifndef __INCLUDED_PARTICLE_OBJECT_H__
#define __INCLUDED_PARTICLE_OBJECT_H__

#include "Projectile.h"
#include "TAKOEngine/Rendering/ParticleRenderer/HitParticleRenderer.h"

/**************************************************************************//**
	@class	ParticleObject
	@brief	飛翔体としてパーティクルクラス
	@par    [説明]
		範囲で強い魔法の攻撃
*//***************************************************************************/
class ParticleObject : public Projectile
{
public:
	ParticleObject(PlayerCharacter* owner);
	~ParticleObject() = default;

	void Update(float elapsedTime) override;

	void SetPosition(const DirectX::XMFLOAT3& position) override { tempPosition = this->position = position; };

	void Collision() override;

	void OnHitEnemy(HitResult& hit) override;

	void RenderDX12(const RenderContextDX12& rc) override;
private:
	DirectX::XMFLOAT3 tempPosition = {}; // 1フレーム前の位置
	float timer = 2.0f; // 寿命

	std::unique_ptr<HitParticleRenderer> m_particle;
	float m_distance = 5.0f;
};

#endif // __INCLUDED_PARTICLE_OBJECT_H__
