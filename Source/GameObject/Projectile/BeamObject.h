//! @file BeamObject.h
//! @note

#ifndef __INCLUDED_BEAM_OBJECT_H__
#define __INCLUDED_BEAM_OBJECT_H__

#include "Projectile.h"

/**************************************************************************//**
	@class	BeamObject
	@brief	飛翔体として火球クラス
	@par    [説明]
		範囲で弱い魔法の攻撃
*//***************************************************************************/
class BeamObject : public Projectile
{
public:
	BeamObject(PlayerCharacter* owner);
	~BeamObject() = default;

	void Update(float elapsedTime) override;
	void CollisionFunction(Collider* myCol, Collider* otherCol) override;
private:
	bool m_isExtend = true; // 伸びている途中か
	float m_distance = 0.5f;
	float m_length = 15.0f;
	float m_radius = 1.0f;
};

#endif // __INCLUDED_FIREBALL_OBJECT_H__
