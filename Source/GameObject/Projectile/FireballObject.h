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
	void CollisionFunction(Collider* myCol, Collider* otherCol) override;

	void RenderDX12(const RenderContextDX12& rc) override;
private:
	PlayerCharacter* m_pOwner;
	float m_radius = 0.0f;
	float m_chargeTime = 0.0f;
	bool m_isCharge = true;	// チャージ中かどうか

	std::unique_ptr<PlaneDX12> m_fireball;
};

#endif // __INCLUDED_FIREBALL_OBJECT_H__
