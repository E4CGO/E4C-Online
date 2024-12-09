//! @file PlayerCharacterShieldState.cpp
//! @note 

#include "PlayerCharacterShieldState.h"


namespace PlayerCharacterState
{
	namespace Shield
	{
		// 特殊攻撃ステート
		void AttackSpecialState::Enter()
		{
			owner->SetAnimationSpeed(1.2f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_START, false, 0.05f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_CONTINUE, true);
		}
		void AttackSpecialState::Execute(float elapsedTime)
		{
			owner->StopMove();

			if (!owner->InputGuard())
			{
				owner->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::STATE::IDLE));
			}
		}
		void AttackSpecialState::Exit()
		{
			owner->SetAnimationSpeed(1.0f);
			owner->SetAnimation(PlayerCharacter::Animation::ANIM_GUARD_SHIELD_FINISH, false, 0.05f);
		}
	}
}