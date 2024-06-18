#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/PlayerState.h"

namespace RogueState
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
	private:
		bool isShot = false;
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
	private:
		bool isShot = false;
	};
	// 特殊攻撃(エイム)
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
		void Exit() override {};
	};

	// 特殊攻撃(発射)
	class AttackSpecialStateShot : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		AttackSpecialStateShot(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~AttackSpecialStateShot() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	private:
		bool isShot = false;
	};

	// スキル_1 手榴弾
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

	// スキル_1エイム
	class Skill1StateAim : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		Skill1StateAim(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~Skill1StateAim() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
	// スキル_1投げ
	class Skill1StateAttack : public HierarchicalState<Player>
	{
	public:
		// コンストラクタ
		Skill1StateAttack(Player* player) : HierarchicalState<Player>(player) {};
		// デストラクタ
		~Skill1StateAttack() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	private:
		bool isShot = false;
	};
}