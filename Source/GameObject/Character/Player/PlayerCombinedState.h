#pragma once
#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/CombinedPlayer.h"

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

namespace PlayerCombinedState
{
	void PlayerTransition(CombinedPlayer* owner, uint32_t flags);

	// 待機ステートオブジェクト
	class IdleState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		IdleState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class MoveState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		MoveState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class JumpState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		JumpState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class FallState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		FallState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class LandState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		LandState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class DodgeState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		DodgeState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class HurtState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		HurtState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class DeathState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		DeathState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class WaitState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		WaitState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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
	class ReadyState : public HierarchicalState<CombinedPlayer>
	{
	public:
		// コンストラクタ
		ReadyState(CombinedPlayer* player) : HierarchicalState<CombinedPlayer>(player) {};
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