//! @file PigMobState.h
//! @note 

#ifndef __INCLUDED_PIG_MOB_STATE_H__
#define __INCLUDED_PIG_MOB_STATE_H__

#include "PigMob.h"

namespace EnemyState
{
	namespace PigMob
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
			IdleState(::PigMob* enemy, float waitTime = 3.0f) : EnemyState::IdleState(enemy, waitTime), owner(enemy) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			virtual void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
		private:
			::PigMob* owner;
		};

		/**************************************************************************//**
			@class	MoveState
			@brief	徘徊ステート
			@par    [説明]
				徘徊の状態
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
			@class	ReloadState
			@brief	リロードステート
			@par	[説明]
				コインを入れて行動開始
		*//***************************************************************************/
		class ReloadState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			ReloadState(::PigMob* enemy) : HierarchicalState<Enemy>(enemy), owner(enemy) {};
			// デストラクタ
			~ReloadState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		private:
			::PigMob* owner;
		};

		/**************************************************************************//**
			@class	AwayState
			@brief	距離を取るステート
			@par	[説明]
				ターゲットから離れる
		*//***************************************************************************/
		class AwayState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			AwayState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
			// デストラクタ
			~AwayState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};

		/**************************************************************************//**
			@class	ShotState
			@brief	射撃ステート
			@par	[説明]
				ターゲットに向けてコインを発射
		*//***************************************************************************/
		class ShotState : public HierarchicalState<Enemy>
		{
		public:
			// コンストラクタ
			ShotState(::PigMob* enemy) : HierarchicalState<Enemy>(enemy), owner(enemy) {};
			// デストラクタ
			~ShotState() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override;
		private:
			::PigMob* owner;
			bool m_isShot = false;
			float m_tempTurnSpeed = 0.0f;
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

#endif