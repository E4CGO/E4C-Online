//! @file BirdMobState.h
//! @note

#ifndef __INCLUDE_BIRD_MOB_STATE_H__
#define __INCLUDE_BIRD_MOB_STATE_H__

#include "BirdMob.h"
#include "EnemyState.h"

namespace EnemyState
{
	namespace BirdMob
	{
		// 待機ステート
		class IdleState : public EnemyState::IdleState
		{
		public:
			// コンストラクタ
			IdleState(Enemy* enemy, float waitTime = 2.0f) : EnemyState::IdleState(enemy, waitTime) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		};

		// エンカウンターステート
		class EncounterState : public HierarchicalState<Enemy>
		{
		public:
			EncounterState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			~EncounterState() {}

			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			void Exit() override {}
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
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		};

		// 怪我ステート
		class HurtState : public EnemyState::HurtState
		{
		public:
			HurtState(Enemy* enemy) : EnemyState::HurtState(enemy) {};
			~HurtState() {}

			void Enter() override {
				owner->SetAnimation(::BirdMob::ANIM_HURT, false);
			}
		};

		// 死亡ステート
		class DeathState : public EnemyState::DeathState
		{
		public:
			DeathState(Enemy* enemy) : EnemyState::DeathState(enemy) {};
			~DeathState() {}

			void Enter() override {
				EnemyState::DeathState::Enter();
				owner->SetAnimation(::BirdMob::ANIM_DEATH, false);
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
		};
		// 攻撃移動ステート
		class AttackMoveState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackMoveState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackMoveState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			DirectX::XMFLOAT3 m_startPos = {};
			DirectX::XMFLOAT3 m_targetPos = {};
		};
		// 攻撃ダイブステート
		class AttackDiveState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackDiveState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackDiveState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;

			struct BIRD_ATTACK
			{
				int power = 5;
				uint8_t idx = ::BirdMob::COLLIDER_ID::COL_ATTACK;
				Collider::COLLIDER_OBJ objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
				uint16_t hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
				float hitStartRate = 0.0f;
				float hitEndRate = 1.0f;
				Sphere sphere{ {0, 60.0f, 0}, 0.5f };
			};

		private:
			BIRD_ATTACK birdAttack;
		};
		// 攻撃着地ステート
		class AttackLandingState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackLandingState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackLandingState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override {}
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		// 飛ぶ上げステート
		class TakeOffState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			TakeOffState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~TakeOffState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			DirectX::XMFLOAT3 m_startPos = {};
		};
	}
}

#endif // !__INCLUDE_BIRD_MOB_STATE_H__
