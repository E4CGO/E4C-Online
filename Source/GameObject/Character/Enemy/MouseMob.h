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
	MouseMob(float scaling = 0.5f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11);
	~MouseMob() = default;
public:
	enum COLLIDER_ID : uint8_t
	{
		COL_ATTACK,
		COL_BODY,
	};

protected:
	void UpdateTarget();
};

#endif //!__INCLUDED_MOUSE_MOB_H__
