//! @file PlayerCharacterRodState.h
//! @note

#ifndef __INCLUDED_PLAYER_CHARACTER_ROD_STATE_H__
#define __INCLUDED_PLAYER_CHARACTER_ROD_STATE_H__

#include "GameObject/Character/Player/PlayerCharacter.h"
#include "Source/GameObject/Projectile/ProjectileManager.h"
#include "Source/GameObject/Projectile/FireballObject.h"
#include "Source/GameObject/Projectile/ParticleObject.h"

namespace PlayerCharacterState
{
	namespace Rod
	{
		enum NORMAL_ATTACK_STATE {
			ATTACK_1,
			ATTACK_2,
			ATTACK_3,
		};

		extern bool m_IsShot;

		// 待機用ステートオブジェクト
		class WaitState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			WaitState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~WaitState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};

		class IdleState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			IdleState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime);
			// ステートから出ていくときのメソッド
			void Exit() override;
		};

		// 移動ステートオブジェクト
		class MoveState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			MoveState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~MoveState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime);
			// ステートから出ていくときのメソッド
			void Exit() override;
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

			Projectile* m_particle = nullptr;
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
#endif // !__INCLUDED_PLAYER_CHARACTER_ROD_STATE_H__