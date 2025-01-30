//! @file PigMob.h
//! @note 豚敵

#ifndef __INCLUDED_PIG_MOB_H__
#define __INCLUDED_PIG_MOB_H__

#include "Enemy.h"
#include "EnemyState.h"

/**************************************************************************//**
	@class　PigMob
	@brief	豚敵を作成、管理
	@par    [説明]
			豚敵の設定クラス
*//***************************************************************************/
class PigMob : public Enemy
{
public:
	PigMob(float scaling = 0.5f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11);
	~PigMob() = default;
public:
	enum STATE
	{
		WANDER = Enemy::STATE::END,
		RELOARD,
		AWAY,
		SHOT,
	};

	enum COLLIDER_ID : uint8_t
	{
		COL_BODY,
	};

	enum ANIMATION : uint8_t
	{
		ANIM_IDLE,
		ANIM_MOVE,
		ANIM_RELOAD,
		ANIM_SHOT,
		ANIM_DAMAGE,
		ANIM_DIE,
	};
	void OnWall() override;
	void Shot();
	bool IsReloaded() const { return m_isReloaded; }
	void SetReload(bool reloaded = true);
private:
	float m_tempMoveSpeed;
	bool m_isReloaded = false; // リロード済判定
};

#include "GameObject/Projectile/ProjectileManager.h"
/**************************************************************************//**
	@class　CoinBullet
	@brief	コイン弾丸クラス
	@par    [説明]
*//***************************************************************************/
class CoinBullet : public Projectile
{
public:
	CoinBullet(DirectX::XMFLOAT3 target, Character* owner);
	~CoinBullet() = default;

	void Update(float elapsedTime) override;
	void CollisionFunction(Collider* myCol, Collider* otherCol) override;
private:
	float m_radius;
	float m_turnSpeed;
};
#endif //!__INCLUDED_PIG_MOB_H__
