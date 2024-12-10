#pragma once

#include "TAKOEngine/AI/BaseState.h"
#include "GameObject/Character/Enemy/Enemy.h"

namespace enemy
{
	enum STATE {			
		IDLE,
		TARGET_FOUND,

		HURT,
		DEATH,

		END,
	};

	// 待機ステートオブジェクト
	class IdleState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		IdleState(Enemy* enemy, float waitTime = 2.0f) : HierarchicalState<Enemy>(enemy), waitTime(waitTime) {};
		// デストラクタ
		~IdleState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float waitTime;
		float waitTimer = 0.0f;
	};

	// 移動ステート
	class MoveState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		MoveState(Enemy* enemy, DirectX::XMFLOAT3 position, int nextState = STATE::IDLE) : HierarchicalState<Enemy>(enemy), position(position), nextState(nextState) {};
		// デストラクタ
		~MoveState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		DirectX::XMFLOAT3 position;
		int nextState;
	};

	// 追跡ステート
	class FollowState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		FollowState(Enemy* enemy, float distance, int nextState = STATE::IDLE) : HierarchicalState<Enemy>(enemy), distance(distance), nextState(nextState) {};
		// デストラクタ
		~FollowState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float distance;
		int nextState;
	};

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
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

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
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}