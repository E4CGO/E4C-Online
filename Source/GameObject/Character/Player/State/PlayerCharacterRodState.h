//! @file PlayerCharacterRodState.h
//! @note

#ifndef __INCLUDED_PLAYER_CHARACTER_ROD_STATE_H__
#define __INCLUDED_PLAYER_CHARACTER_ROD_STATE_H__

#include "GameObject/Character/Player/PlayerCharacter.h"
#include "Source/GameObject/Projectile/ProjectileManager.h"
#include "Source/GameObject/Projectile/FireballObject.h"
#include "Source/GameObject/Projectile/ParticleObject.h"
#include "Source/GameObject/Projectile/BeamObject.h"

namespace PlayerCharacterState
{
	namespace Rod
	{
		enum FIREBALL_STATE {
			CHARGE_READY,
			CHARGE,
			ATTACK_1,
			ATTACK_2,
			ATTACK_3,
			ATTACK_END,
		};

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

		// Fireball攻撃
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
		// Fireball攻撃準備
		class AttackNormalState_Ready : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_Ready(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_Ready() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
		// Fireball攻撃チャージ
		class AttackNormalState_Charge : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_Charge(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_Charge() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};

		private:
			float m_chargeTme = 0.0f;
			const float MAX_ChargeTime = 4.0f;
			uint16_t m_power = 50;
		};
		// Fireball小攻撃
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
		// Fireball中攻撃
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
		// Fireball大攻撃
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
		// Fireball攻撃終了
		class AttackNormalState_End : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_End(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~AttackNormalState_End() {}
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

		// スキル_1 ビーム
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
			DirectX::XMFLOAT3 m_dir = {0, 0, 0};
			bool m_isShot = false;
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