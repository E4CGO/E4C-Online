#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/PlayerState.h"

namespace BarbarianState
{
	// 待機ステートオブジェクト
	class IdleState : public PlayerState::IdleState
	{
	public:
		// コンストラクタ
		IdleState(Player* player) : PlayerState::IdleState(player) {};
		// デストラクタ
		~IdleState() {}
		// ステートに入った時のメソッド
		void Enter() override;
	};

	// 待機用ステートオブジェクト
	class WaitState : public PlayerState::WaitState
	{
	public:
		// コンストラクタ
		WaitState(Player* player) : PlayerState::WaitState(player) {};
		// デストラクタ
		~WaitState() {}
		// ステートに入った時のメソッド
		void Enter() override;
	};

	// 一般攻撃
	class AttackNormalState : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackNormalState(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackNormalState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
	// 一般攻撃1
	class AttackNormalState_1 : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackNormalState_1(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackNormalState_1() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	// 一般攻撃2
	class AttackNormalState_2 : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackNormalState_2(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackNormalState_2() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	// 一般攻撃3
	class AttackNormalState_3 : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackNormalState_3(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackNormalState_3() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};

	// 特殊攻撃
	class AttackSpecialState : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialState(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 特殊攻撃エイム
	class AttackSpecialStateAim : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateAim(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateAim() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
	// 特殊攻撃投げ
	class AttackSpecialStateAttack : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateAttack(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateAttack() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	private:
		bool isShot = false;
	};

	// スキル_1 回レ
	class Skill1State : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		Skill1State(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~Skill1State() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	private:
		float cacheTurnSpeed = 0.0f;
	};
	class Skill1StateStart : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		Skill1StateStart(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~Skill1StateStart() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	class Skill1StateLoop : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		Skill1StateLoop(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~Skill1StateLoop() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}