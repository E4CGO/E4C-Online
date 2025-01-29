//! @file FireballObject.h
//! @note

#ifndef __INCLUDED_FIREBALL_OBJECT_H__
#define __INCLUDED_FIREBALL_OBJECT_H__

#include "Projectile.h"
#include "TAKOEngine/Rendering/Plane.h"

/**************************************************************************//**
	@class	FireballObject
	@brief	飛翔体として火球クラス
	@par    [説明]
		範囲で弱い魔法の攻撃
*//***************************************************************************/
class FireballObject : public Projectile
{
public:
	FireballObject(PlayerCharacter* owner);
	~FireballObject() = default;

	void Update(float elapsedTime) override;

	void SetPosition(const DirectX::XMFLOAT3& position) override { tempPosition = this->position = position; };

	void Collision() override;

	void OnHitEnemy(HitResult& hit) override;

	void RenderDX12(const RenderContextDX12& rc) override;
private:
	DirectX::XMFLOAT3 tempPosition = {}; // 1フレーム前の位置
	float timer = 2.0f; // 寿命

	std::unique_ptr<PlaneDX12> m_fireball;
};

#endif // __INCLUDED_FIREBALL_OBJECT_H__
