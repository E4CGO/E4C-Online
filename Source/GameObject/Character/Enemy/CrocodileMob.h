//! @file CrocodileMob.h
//! @note 鰐敵

#ifndef __INCLUDED_CROCODILE_MOB_H__
#define __INCLUDED_CROCODILE_MOB_H__

#include "Enemy.h"
#include "EnemyState.h"

/**************************************************************************//**
	@class　CrocodileMob
	@brief	鰐敵を作成、管理
	@par    [説明]
			鰐敵の設定クラス
*//***************************************************************************/
class CrocodileMob : public Enemy
{
public:
	CrocodileMob(float scaling = 0.5f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11);
	~CrocodileMob() = default;
public:
	enum STATE
	{
		WANDER = Enemy::STATE::END,
		FOLLOW,
		ATTACK,
	};

	enum COLLIDER_ID : uint8_t
	{
		COL_ATTACK,
		COL_BODY,
	};

	enum ANIMATION : uint8_t
	{
		ANIM_IDLE,
		ANIM_MOVE,
		ANIM_ATTACK,
		ANIM_DAMAGE,
		ANIM_DIE,
	};
};

#endif //!__INCLUDED_CROCODILE_MOB_H__
