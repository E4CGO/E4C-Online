//! @file PlayerCharacterState.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_STATE__
#define __INCLUDED_PLAYER_CHARACTER_STATE__

#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Player/PlayerCharacter.h"

// FLAG

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


	// いつか消す
	namespace Axe
	{
		// 待機ステートオブジェクト
		class IdleState : public PlayerCharacterState::IdleState
		{
		public:
			// コンストラクタ
			IdleState(PlayerCharacter* player) : PlayerCharacterState::IdleState(player) {};
			// デストラクタ
			~IdleState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 待機用ステートオブジェクト
		class WaitState : public PlayerCharacterState::WaitState
		{
		public:
			// コンストラクタ
			WaitState(PlayerCharacter* player) : PlayerCharacterState::WaitState(player) {};
			// デストラクタ
			~WaitState() {}
			// ステートに入った時のメソッド
			void Enter() override;
		};

		// 一般攻撃
		class AttackNormalState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackNormalState_1 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_1(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackNormalState_2 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_2(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackNormalState_3 : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackNormalState_3(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackSpecialState : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackSpecialState(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackSpecialStateAim : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackSpecialStateAim(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class AttackSpecialStateAttack : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			AttackSpecialStateAttack(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class Skill1State : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill1State(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
		class Skill1StateStart : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill1StateStart(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
			// デストラクタ
			~Skill1StateStart() {}
			// ステートに入った時のメソッド
			void Enter() override;
			// ステートで実行するメソッド
			void Execute(float elapsedTime) override;
			// ステートから出ていくときのメソッド
			void Exit() override {};
		};
		class Skill1StateLoop : public HierarchicalState<PlayerCharacter>
		{
		public:
			// コンストラクタ
			Skill1StateLoop(PlayerCharacter* player) : HierarchicalState<PlayerCharacter>(player) {};
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
}
#endif // !__INCLUDED_PLAYER_CHARACTER_STATE__
