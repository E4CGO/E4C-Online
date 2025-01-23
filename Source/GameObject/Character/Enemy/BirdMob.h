//! @file BirdMob.h
//! @note 

#ifndef __INCLUDE_BIRD_MOB_H__
#define __INCLUDE_BIRD_MOB_H__

#include "Enemy.h"

/**************************************************************************//**
	@class　MouseMob
	@brief	ネズミ敵を作成、管理
	@par    [説明]
			ネズミ敵の設定クラス
*//***************************************************************************/
class BirdMob : public Enemy
{
public:
	BirdMob(float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX12);
	~BirdMob() = default;
public:
	// アニメーション
	enum ANIMATION : uint8_t {
		ANIM_IDLE,			// 待機
		ANIM_ENCOUNTER,		// エンカウンター
		ANIM_ATTACK_1,		// 飛ぶ
		ANIM_ATTACK_2,		// ダイヴィング
		ANIM_ATTACK_3,		// 着地
		ANIM_HURT,			// 怪我
		ANIM_DEATH,			// 死亡
	};

	enum STATE : uint8_t {
		WANDER = Enemy::STATE::END,
		ENCOUNTER,
		FOLLOW,
		ATTACK,
		TAKE_OFF,
	};

	enum ATTACK_STATE : uint8_t {
		ATTACK_MOVE,
		ATTACK_DIVE,
		ATTACK_LAND,
	};
};

#endif // !__INCLUDE_BIRD_MOB_H__
