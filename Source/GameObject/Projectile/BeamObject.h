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

	//void SetPosition(const DirectX::XMFLOAT3& position) override { tempPosition = this->position = position; };

	//void Collision() override;

	//void OnHitEnemy(HitResult& hit) override;

	void RenderDX12(const RenderContextDX12& rc) override;
private:
	//DirectX::XMFLOAT3 tempPosition = {}; // 1フレーム前の位置
	//float timer = 2.0f; // 寿命
	bool m_isExtend = true; // 伸びている途中か
	float m_distance = 0.5f;
	float m_length = 15.0f;
	float m_radius = 1.0f;
	//float m_distance = 5.0f;
	//std::unique_ptr<ModelObject> m_beam;
};

#endif // __INCLUDED_FIREBALL_OBJECT_H__
