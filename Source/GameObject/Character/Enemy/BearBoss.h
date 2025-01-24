//! @file BearBoss.h
//! @note 

#ifndef __INCLUDED_BEAR_BOSS_H__
#define __INCLUDED_BEAR_BOSS_H__

#include "Enemy.h"
#include "EnemyState.h"

/**************************************************************************//**
	@class　BearBoss
	@brief	ボスクマを作成、管理
	@par    [説明]
			ボスクマの設定クラス
*//***************************************************************************/
class BearBoss : public Enemy
{
public:
	BearBoss(float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX12);
	~BearBoss() = default;
public:
	enum COLLIDER_ID : uint8_t
	{
		COL_BODY,
		COL_HEAD,
		COL_BUTTON,
		COL_LEFT_HAND,
		COL_RIGHT_HAND,
		COL_ATTACK_LEFT_HAND,
		COL_ATTACK_RIGHT_HAND,
		COL_LEFT_LEG,
		COL_RIGHT_LEG,
	};

	enum ANIMATION : uint8_t
	{
		ANIM_IDLE,
		ANIM_WALK_START,
		ANIM_WALK_LOOP,
		ANIM_PUNCH,
	};

	enum STATE : uint8_t
	{
		WANDER = Enemy::STATE::END,
		ATTACK,
	};

	enum ATTACK_STATE : uint8_t
	{
		FOLLOW = 0,
		PUNCH,
	};

protected:
	void UpdateTarget() override;
};


#include "GameObject/Projectile/ProjectileManager.h"
/**************************************************************************//**
	@class　PunchImpact
	@brief	ボスクマのパンチ衝撃波クラス
	@par    [説明]
*//***************************************************************************/
class PunchImpact : public Projectile
{
public:
	PunchImpact(DirectX::XMFLOAT3 pos, Character* owner);
	~PunchImpact() = default;

	void Update(float elapsedTime) override;
	void CollisionFunction(Collider* myCol, Collider* otherCol);

	void RenderDX12(const RenderContextDX12& rc) override
	{
		if (!m_pColliders.empty())
		{
			for (const std::pair<uint8_t, Collider*>& attackCollider : m_pColliders)
			{
				DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
				if (attackCollider.second->GetOBJType() & (Collider::COLLIDER_OBJ::PLAYER | Collider::COLLIDER_OBJ::ENEMY | Collider::COLLIDER_OBJ::ITEM))
				{
					color = { 0, 1, 0, 1 };
				}
				else if (attackCollider.second->GetOBJType() & (Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE | Collider::COLLIDER_OBJ::ENEMY_ATTACK | Collider::COLLIDER_OBJ::ENEMY_PROJECTILE))
				{
					color = { 1, 0, 0, 1 };
				}

				attackCollider.second->DrawDebugPrimitive(color);
			}
		}
	}

private:
	float time = 0.0f;
	const float existTime = 0.5f;
	const float maxRadius = 5.0f;
	const float minRadius = 1.4f;
};


#endif // !__INCLUDED_BEAR_BOSS_H__
