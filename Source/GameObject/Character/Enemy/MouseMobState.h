//! @file MouseMobState.h
//! @note ネズミ敵ステート

#ifndef __INCLUDED_MOUSE_MOB_STATE_H__
#define __INCLUDED_MOUSE_MOB_STATE_H__

#include "MouseMob.h"
namespace EnemyState
{
	namespace MouseMob
	{
		/**************************************************************************//**
			@class	IdleState
			@brief	待機ステート
			@par    [説明]
				待機の状態
		*//***************************************************************************/
		class IdleState : public EnemyState::IdleState
		{
		public:
			// コンストラクタ
			//IdleState(Enemy* enemy, float minWaitingTime = 3.0f, float maxWaitingTime = 5.0f) :
			//	HierarchicalState<Enemy>(enemy), m_MinWaitingTime(minWaitingTime), m_MaxWaitingTime(maxWaitingTime) {};
			IdleState(Enemy* enemy, float waitTime = 3.0f) : EnemyState::IdleState(enemy, waitTime) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		};

		/**************************************************************************//**
			@class	MoveState
			@brief	徘徊ステート
			@par    [説明]
				徘徊
		*//***************************************************************************/
		class MoveState : public EnemyState::MoveState
		{
		public:
			// コンストラクタ
			MoveState(Enemy* enemy, int nextState = Enemy::STATE::IDLE) : EnemyState::MoveState(enemy, nextState) {};
			// デストラクタ
			~MoveState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		};

		/**************************************************************************//**
			@class	EncounterState
			@brief	エンカウントステート
			@par    [説明]
				プレイヤーを発見したリアクションをとる
		*//***************************************************************************/
		class EncounterState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			EncounterState(Enemy* enemy) : HierarchicalState<Enemy>(enemy){};
			// デストラクタ
			~EncounterState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};

		/**************************************************************************//**
			@class	AttackState
			@brief	攻撃ステート
			@par    [説明]
				プレイヤーを攻撃する
		*//***************************************************************************/
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
			void Execute(float elapsedTime);
			// ステートから出ていくときのメソッド
			void Exit() override;

			struct MOUSE_ATTACK
			{
				int power = 5;
				uint8_t idx = ::MouseMob::COLLIDER_ID::COL_ATTACK;
				Collider::COLLIDER_OBJ objType = Collider::COLLIDER_OBJ::ENEMY_ATTACK;
				uint16_t hittableOBJ = Collider::COLLIDER_OBJ::PLAYER;
				float hitStartRate = 7.0f / 17.0f;
				float hitEndRate = 73.0f / 170.0f;
				Capsule capsule{ {120.0f, 0, 0}, {0, 1, 0}, 1.0f, 0.6f };
			};

		private:
			MOUSE_ATTACK mouseAttack;
		};

		/**************************************************************************//**
			@class	HurtState
			@brief	ダメージステート
			@par    [説明]
				ダメージステート
		*//***************************************************************************/
		class HurtState : public EnemyState::HurtState
		{
		public:
			// コンストラクタ
			HurtState(Enemy* enemy) : EnemyState::HurtState(enemy) {};
			// デストラクタ
			~HurtState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		/**************************************************************************//**
			@class	DeathState
			@brief	死亡ステート
			@par    [説明]
				死亡ステート
		*//***************************************************************************/
		class DeathState : public EnemyState::DeathState
		{
		public:
			// コンストラクタ
			DeathState(Enemy* enemy) : EnemyState::DeathState(enemy) {};
			// デストラクタ
			~DeathState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};
	}
}

#endif //!__INCLUDED_MOUSE_MOB_STATE_H__