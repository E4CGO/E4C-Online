//! @file PlayerCharacterState.h
//! @note

#ifndef __INCLUDED_PLAYER_CHARACTER_SWORD_STATE__
#define __INCLUDED_PLAYER_CHARACTER_SWORD_STATE__

#include "GameObject/Character/Player/PlayerCharacter.h"

namespace PlayerCharacterState
{
	namespace Sword
	{
		enum NORMAL_ATTACK_STATE {
			ATTACK_1,
			ATTACK_2,
			ATTACK_3,
		};

		enum SKILL_1_STATE
		{
			ATTACK_START,
			ATTACK_CONTINUE,
		};

		// 一般攻撃
		class AttackNormalState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		// 一般攻撃1
		class AttackNormalState_1 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_1(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_1() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
		// 一般攻撃2
		class AttackNormalState_2 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_2(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_2() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
		// 一般攻撃3
		class AttackNormalState_3 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_3(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_3() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};

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

		// スキル_1 回レ
		class Skill1State : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill1State(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~Skill1State() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			float cacheTurnSpeed = 0.0f;
		};

		class Skill1StateStart : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill1StateStart(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~Skill1StateStart() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};

		class Skill1ContinueStart : public HierarchicalState<PlayerCharacter>
		{
		public:

			float impulseSpeed = 50.0f;

			// コンストラクタ
			Skill1ContinueStart(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~Skill1ContinueStart() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};

		class Skill2State : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill2State(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~Skill2State() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
	}
}
#endif // !__INCLUDED_PLAYER_CHARACTER_SWORD_STATE__