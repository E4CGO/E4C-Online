﻿//! @file BearBossState.h
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
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
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
		// 攻撃追跡ステート
		class AttackFollowState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackFollowState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackFollowState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};
		// 攻撃パンチステート
		class AttackPunchState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AttackPunchState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AttackPunchState() {}
			// ステートに入った時のメソッド
			void Enter() override;
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

		private:
			void UpdatePunchImpact(float elapsedTime);

		private:
			ATTACK_ARM attackArms[2];
			bool impacts[2] = { false, false };

			const float maxFlame = 572.0f; // attackのアニメーション:572フレーム
			const float impactFlame1 = 165.0f;
			const float impactFlame2 = 270.0f;
			const float impact1StartRate = impactFlame1 / maxFlame;
			const float impact2StartRate = impactFlame2 / maxFlame;
		};
	}
}

#endif // !__INCLUDED_BEAR_BOSS_STATE_H__