//! @file SkeletonMinionState.h
//! @note 

#pragma once

#include "EnemyState.h"
#include "SkeletonMinion.h"

namespace SkeletonMinionState {
	// 待機ステートオブジェクト
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
}
