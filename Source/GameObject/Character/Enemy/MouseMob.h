//! @file MouseMob.h
//! @note ネズミ敵

#ifndef __INCLUDED_MOUSE_MOB_H__
#define __INCLUDED_MOUSE_MOB_H__

#include "Enemy.h"
#include "EnemyState.h"

/**************************************************************************//**
	@class　MouseMob
	@brief	ネズミ敵を作成、管理
	@par    [説明]
			ネズミ敵の設定クラス
*//***************************************************************************/
class MouseMob : public Enemy
{
public:
	MouseMob(float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX12);
	~MouseMob() = default;
private:
	void UpdateColliders() override;
public:
	enum AttackCollider
	{
		BodyAtc
	};

	enum State
	{
		Search = EnemyState::ID::End,
		Battle,
		Recieve,
	};

protected:
	enum HitCollider
	{
		BodyHit
	};

	void UpdateTarget();
};

#endif //!__INCLUDED_MOUSE_MOB_H__
