#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/PlayerState.h"

namespace KnightState
{
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

	// 盾持ち上げ
	class AttackSpecialStateStart : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateStart(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateStart() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	// 盾持ち（ループ）
	class AttackSpecialStateBlocking : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateBlocking(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateBlocking() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	// 盾攻撃
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
		void Exit() override;
	};
	// 盾受け
	class AttackSpecialStateHit : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateHit(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateHit() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};

	// スキル_1 突進
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
	};
}