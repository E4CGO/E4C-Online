//! @file BearBossState.h
//! @note 

#ifndef __INCLUDED_BEAR_BOSS_STATE_H__
#define __INCLUDED_BEAR_BOSS_STATE_H__

#include "BearBoss.h"

namespace EnemyState
{
	namespace BearBoss
	{
		class IdleState : public EnemyState::IdleState
		{
		public:
			// コンストラクタ
			IdleState(Enemy* enemy, float waitTime = 2.0f) : EnemyState::IdleState(enemy, waitTime) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		};
		class AngryIdleState : public IdleState
		{
		public:
			// コンストラクタ
			AngryIdleState(Enemy* enemy, float waitTime = 2.0f) : IdleState(enemy, waitTime) {};
			// デストラクタ
			~AngryIdleState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 徘徊ステート
		class MoveState : public EnemyState::MoveState
		{
		public:
			// コンストラクタ
			MoveState(Enemy* enemy) : EnemyState::MoveState(enemy) {};
			// デストラクタ
			~MoveState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
		};
		class AngryMoveState : public MoveState
		{
		public:
			// コンストラクタ
			AngryMoveState(Enemy* enemy) : MoveState(enemy) {};
			// デストラクタ
			~AngryMoveState() {};
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 追跡ステート
		class FollowState : public EnemyState::FollowState
		{
		public:
			// コンストラクタ
			FollowState(Enemy* enemy) : EnemyState::FollowState(enemy, 0.0f, ::BearBoss::STATE::ATTACK) {};
			// デストラクタ
			~FollowState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
		};
		class AngryFollowState : public FollowState
		{
		public:
			// コンストラクタ
			AngryFollowState(Enemy* enemy) : FollowState(enemy) {};
			// デストラクタ
			~AngryFollowState() {}
			// ステートに入った時のメソッド
			void Enter() override
			{
				FollowState::Enter();
				owner->SetAnimation(::BearBoss::ANIM_WALK_LOOP2, true);
			}
		};

		// 攻撃ステート
		class AttackState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;

			struct ATTACK_ARM
			{
				int power = 0;
				uint8_t idx = 0;
				Collider::COLLIDER_OBJ objType = Collider::COLLIDER_OBJ::HIT_ERR;
				uint16_t hittableOBJ = 0;
				float hitStartRate = 0.0f;
				float hitEndRate = 1.0f;
				Capsule capsule{};
			};

		protected:
			void UpdatePunchImpact(float elapsedTime);

		protected:
			ATTACK_ARM attackArms[2];
			bool impacts[2] = { false, false };

			const float maxFlame = 572.0f; // attackのアニメーション:572フレーム
			const float impactFlame1 = 165.0f;
			const float impactFlame2 = 270.0f;
			const float impact1StartRate = impactFlame1 / maxFlame;
			const float impact2StartRate = impactFlame2 / maxFlame;
		};
		class AngryAttackState : public AttackState
		{
		public:
			// コンストラクタ
			AngryAttackState(Enemy* enemy) : AttackState(enemy) {};
			// デストラクタ
			~AngryAttackState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// スタンステート
		class StunState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			StunState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~StunState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		class StunStartState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			StunStartState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~StunStartState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		class AngryStunStartState : public StunStartState
		{
		public:
			// コンストラクタ
			AngryStunStartState(Enemy* enemy) : StunStartState(enemy) {};
			// デストラクタ
			~AngryStunStartState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			
		};

		class StunLoopState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			StunLoopState(Enemy* enemy, float stunTime = 5.0f) : HierarchicalState<Enemy>(enemy), m_time(stunTime){};
			// デストラクタ
			~StunLoopState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		protected:
			float m_timer = 0.0f;
			float m_time;
		};
		class AngryStunLoopState : public StunLoopState
		{
		public:
			// コンストラクタ
			AngryStunLoopState(Enemy* enemy, float stunTime = 5.0f) : StunLoopState(enemy, stunTime) {};
			// デストラクタ
			~AngryStunLoopState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};
		class StunEndState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			StunEndState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~StunEndState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override {};
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		class AngryStunEndState : public StunEndState
		{
		public:
			// コンストラクタ
			AngryStunEndState(Enemy* enemy) : StunEndState(enemy) {};
			// デストラクタ
			~AngryStunEndState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 咆哮ステート
		class RoarState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			RoarState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~RoarState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
		class AngryRoarState : public RoarState
		{
		public:
			// コンストラクタ
			AngryRoarState(Enemy* enemy) : RoarState(enemy) {};
			// デストラクタ
			~AngryRoarState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 第二段階移行ステート
		class FormChangeState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			FormChangeState(::BearBoss* enemy) : HierarchicalState<Enemy>(enemy), owner(enemy) {};
			// デストラクタ
			~FormChangeState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			::BearBoss* owner;
		};

		// 死亡ステート
		class DeathState : public EnemyState::DeathState
		{
		public:
			DeathState(Enemy* enemy) : EnemyState::DeathState(enemy) {};
			~DeathState() {}

			void Enter() override {
				EnemyState::DeathState::Enter();
				owner->SetAnimation(::BearBoss::ANIM_DEATH, false);
			}
		};
	}
}

#endif // !__INCLUDED_BEAR_BOSS_STATE_H__