#pragma once
#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/NonPlayerCharacter.h"

// FLAG
const uint32_t flag_Dodge = 1 << 0;
const uint32_t flag_Jump = 1 << 1;
const uint32_t flag_Move = 1 << 2;
const uint32_t flag_Stop = 1 << 3;
const uint32_t flag_Fall = 1 << 4;
const uint32_t flag_Ground = 1 << 5;
const uint32_t flag_AttackN = 1 << 6;
const uint32_t flag_AttackS = 1 << 7;
const uint32_t flag_Skill_1 = 1 << 8;
const uint32_t flag_Skill_2 = 1 << 9;
const uint32_t flag_Skill_3 = 1 << 10;
const uint32_t flag_Skill_4 = 1 << 11;

namespace NonPlayerCharacterState
{
	void PlayerTransition(NonPlayerCharacter* owner, uint32_t flags);

	// 待機ステートオブジェクト
	class IdleState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		IdleState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~IdleState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 移動ステートオブジェクト
	class MoveState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		MoveState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~MoveState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// ジャンプステートオブジェクト
	class JumpState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		JumpState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~JumpState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 落下ステートオブジェクト
	class FallState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		FallState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~FallState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 着地ステートオブジェクト
	class LandState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		LandState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~LandState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 回避ステートオブジェクト
	class DodgeState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		DodgeState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~DodgeState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 怪我ステートオブジェクト
	class HurtState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		HurtState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~HurtState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 死亡ステートオブジェクト
	class DeathState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		DeathState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~DeathState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 待機用ステートオブジェクト
	class WaitState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		WaitState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~WaitState() {}
		// ステートに入った時のメソッド
		virtual void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};

	// 待機用 (準備完了)ステートオブジェクト
	class ReadyState : public HierarchicalState<NonPlayerCharacter>
	{
	public:
		// コンストラクタ
		ReadyState(NonPlayerCharacter* player) : HierarchicalState<NonPlayerCharacter>(player) {};
		// デストラクタ
		~ReadyState() {}
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override;
	};
}