#pragma once
#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/PlayerCharacter.h"

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

namespace PlayerCharacterState
{
	void PlayerTransition(PlayerCharacter* owner, uint32_t flags);

	// 待機ステートオブジェクト
	class IdleState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		IdleState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class MoveState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		MoveState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class JumpState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		JumpState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class FallState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		FallState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class LandState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		LandState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class DodgeState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		DodgeState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class HurtState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		HurtState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class DeathState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		DeathState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class WaitState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		WaitState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
	class ReadyState : public HierarchicalState<PlayerCharacter>
	{
	public:
		// コンストラクタ
		ReadyState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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