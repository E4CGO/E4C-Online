//! @file PlayerCharacterShieldState.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_SHIELD_STATE_H__
#define __INCLUDED_PLAYER_CHARACTER_SHIELD_STATE_H__

#include "GameObject/Character/Player/PlayerCharacter.h"
namespace PlayerCharacterState
{
	namespace Shield
	{
		class AttackSpecialState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackSpecialState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackSpecialState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
	}
}


#endif // !__INCLUDED_PLAYER_CHARACTER_SHIELD_STATE_H__
