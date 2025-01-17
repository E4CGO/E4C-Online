//! @file MouseMobState.h
//! @note ネズミ敵ステート

#ifndef __INCLUDED_MOUSE_MOB_STATE_H__
#define __INCLUDED_MOUSE_MOB_STATE_H__

#include "MouseMob.h"
namespace EnemyState
{
	namespace mouseMob {
		enum STATE
		{
			SEARCH = Enemy::STATE::END,
			BATTLE,
			RECIEVE,
		};

		// 移動
		enum SEARCH_STATE
		{
			WANDER,
			IDLE,
		};

		// 攻撃
		enum BATTLE_STATE
		{
			PURSUIT,
			ATTACK,
			STANDBY,
		};

		/**************************************************************************//**
			@class	SearchState
			@brief	探しステート
			@par    [説明]
				待機、移動管理
		*//***************************************************************************/
		class SearchState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			SearchState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~SearchState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};

		/**************************************************************************//**
			@class	BattleState
			@brief	バトルステート
			@par    [説明]
				バトル管理
		*//***************************************************************************/
		class BattleState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			BattleState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~BattleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		};

		/**************************************************************************//**
			@class	WanderState
			@brief	移動ステート
			@par    [説明]
				ランダム位置に移動
		*//***************************************************************************/
		class WanderState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			WanderState(Enemy* enemy, float arrivedRadius = 1.0f) : HierarchicalState<Enemy>(enemy), m_ArrivedRadius(arrivedRadius) {};
			// デストラクタ
			~WanderState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			float m_ArrivedRadius = 1.0f;
		};

		/**************************************************************************//**
			@class	IdleState
			@brief	待機ステート
			@par    [説明]
				待機の状態
		*//***************************************************************************/
		class IdleState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			IdleState(Enemy* enemy, float minWaitingTime = 3.0f, float maxWaitingTime = 5.0f) :
				HierarchicalState<Enemy>(enemy), m_MinWaitingTime(minWaitingTime), m_MaxWaitingTime(maxWaitingTime) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			float m_StateTimer = 0.0f;
			float m_MinWaitingTime;
			float m_MaxWaitingTime;
		};

		/**************************************************************************//**
			@class	PursuitState
			@brief	プレイヤーを追いかけるステート
			@par    [説明]
				プレイヤーに近ずき攻撃を準備
		*//***************************************************************************/
		class PursuitState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			PursuitState(Enemy* enemy,
				float AttackCooldown = 2.0f, float attackRange = 1.25f,
				float minWaitingTime = 3.0f, float maxWaitingTime = 5.0f
			) : HierarchicalState<Enemy>(enemy), m_AttackCooldown(AttackCooldown), m_AttackRange(attackRange),
				m_MinWaitingTime(minWaitingTime), m_MaxWaitingTime(maxWaitingTime) {
			};
			// デストラクタ
			~PursuitState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			float m_AttackCooldown;
			float m_AttackTimer;
			float m_AttackRange;
			float m_StateTimer;
			float m_MinWaitingTime;
			float m_MaxWaitingTime;
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
			AttackState(Enemy* enemy, float waitTimer = .5f) : HierarchicalState<Enemy>(enemy), m_WaitTimer(waitTimer) {};
			// デストラクタ
			~AttackState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime);
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			float m_WaitTimer;
			float m_AnimationTimer;
		};
	}
}

#endif //!__INCLUDED_MOUSE_MOB_STATE_H__