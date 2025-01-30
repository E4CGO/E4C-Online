//! @file EnemyState.h
//! @note 

#ifndef __INCLUDE_ENEMY_STATE_H__
#define __INCLUDE_ENEMY_STATE_H__


#include "TAKOEngine/AI/BaseState.h"
#include "GameObject/Character/Enemy/Enemy.h"

namespace EnemyState
{
	void StateTransition(Enemy* enemy, uint8_t state);

	/**************************************************************************//**
		@class	enemy::IdleState
		@brief	待機ステート基底クラス
		@par	[説明]
				待機ステート基底クラス
	*//***************************************************************************/
	class IdleState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		IdleState(Enemy* enemy, float waitTime = 2.0f) : HierarchicalState<Enemy>(enemy), m_waitTime(waitTime) {};
		// デストラクタ
		~IdleState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		virtual void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		virtual void Exit() override;
	protected:
		bool IsWaiting() { return m_waitTimer > 0.0f; }
	protected:
		float m_waitTime;
		float m_waitTimer = 0.0f;
	};

	/**************************************************************************//**
		@class	enemy::MoveState
		@brief	移動ステート基底クラス
		@par	[説明]
				待機ステート基底クラス
	*//***************************************************************************/
	class MoveState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		MoveState(Enemy* enemy, int nextState = Enemy::STATE::IDLE) : HierarchicalState<Enemy>(enemy), m_nextState(nextState) {};
		// デストラクタ
		~MoveState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		virtual void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		virtual void Exit() override;
	private:
		int m_nextState;
	};

	/**************************************************************************//**
		@class	enemy::FollowState
		@brief	追跡ステート基底クラス
		@par	[説明]
				追跡ステート基底クラス
	*//***************************************************************************/
	class FollowState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		FollowState(Enemy* enemy, float distance, int nextState = Enemy::STATE::IDLE) : HierarchicalState<Enemy>(enemy), m_distance(distance), m_nextState(nextState) {};
		// デストラクタ
		~FollowState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		virtual void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		virtual void Exit() override;
	protected:
		float m_distance;
		int m_nextState;
	};


	/**************************************************************************//**
		@class	enemy::HurtState
		@brief	ダメージステート基底クラス
		@par	[説明]
				ダメージステート基底クラス
	*//***************************************************************************/
	class HurtState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		HurtState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
		// デストラクタ
		~HurtState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		virtual void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		virtual void Exit() override;
	};


	/**************************************************************************//**
		@class	enemy::DeathState
		@brief	死亡ステート基底クラス
		@par	[説明]
				死亡ステート基底クラス
	*//***************************************************************************/
	class DeathState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		DeathState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
		// デストラクタ
		~DeathState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		virtual void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		virtual void Exit() override;
	};
}

#endif // !__INCLUDE_ENEMY_STATE_H__