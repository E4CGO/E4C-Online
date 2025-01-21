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
#endif // !__INCLUDED_BEAR_BOSS_H__
