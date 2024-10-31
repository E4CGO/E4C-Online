#pragma once

#include "TAKOEngine/AI/StateMachine.h"

#include <memory>
#include <unordered_map>

#include "GameObject/Character/Character.h"
#include "GameObject/Character/Player/Player.h"
#include "GameData.h"
#include "PlayerCharacterData.h"

class Enemy;

class NonPlayerCharacter : public Character
{
public:
	// コンストラクタ(引数付き)
	NonPlayerCharacter(uint64_t id, const char* name, uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);
	// コンストラクタ(引数付き)
	NonPlayerCharacter(PlayerCharacterData::CharacterInfo dataInfo);
	// デストラクタ
	~NonPlayerCharacter();

	// KayKit Adventurers
	enum Animation
	{
		OneHanded_Melee_Attack_Chop,
		OneHanded_Melee_Attack_Slice_Diagonal,
		OneHanded_Melee_Attack_Slice_Horizontal,
		OneHanded_Melee_Attack_Stab,
		OneHanded_Ranged_Aiming,
		OneHanded_Ranged_Reload,
		OneHanded_Ranged_Shoot,
		OneHanded_Ranged_Shooting,
		TwoHanded_Melee_Attack_Chop,
		TwoHanded_Melee_Attack_Slice,
		TwoHanded_Melee_Attack_Spin,
		TwoHanded_Melee_Attack_Spinning,
		TwoHanded_Melee_Attack_Stab,
		TwoHanded_Melee_Idle,
		TwoHanded_Ranged_Aiming,
		TwoHanded_Ranged_Reload,
		TwoHanded_Ranged_Shoot,
		TwoHanded_Ranged_Shooting,
		Block,
		Block_Attack,
		Block_Hit,
		Blocking,
		Cheer,
		Death_A,
		Death_A_Pose,
		Death_B,
		Death_B_Pose,
		Dodge_Backward,
		Dodge_Forward,
		Dodge_Left,
		Dodge_Right,
		Dualwield_Melee_Attack_Chop,
		Dualwield_Melee_Attack_Slice,
		Dualwield_Melee_Attack_Stab,
		Hit_A,
		Hit_B,
		Idle,
		Interact,
		Jump_Full_Long,
		Jump_Full_Short,
		Jump_Idle,
		Jump_Land,
		Jump_Start,
		Lie_Down,
		Lie_Idle,
		Lie_Pose,
		Lie_StandUp,
		Pickup,
		Running_A,
		Running_B,
		Running_Strafe_Left,
		Running_Strafe_Right,
		Sit_Chair_Down,
		Sit_Chair_Idle,
		Sit_Chair_Pose,
		Sit_Chair_StandUp,
		Sit_Floor_Down,
		Sit_Floor_Idle,
		Sit_Floor_Pose,
		Sit_Floor_StandUp,
		Spellcast_Charge,
		Spellcast_Long,
		Spellcast_Raise,
		Spellcast_Shoot,
		T_POSE,
		Throw,
		Unarmed_Idle,
		Unarmed_Melee_Attack_Kick,
		Unarmed_Melee_Attack_Punch_A,
		Unarmed_Melee_Attack_Punch_B,
		Unarmed_Pose,
		Use_Item,
		Walking_A,
		Walking_B,
		Walking_Backward,
		Walking_C
	};

	enum class State
	{
		Idle,
		Move,
		Jump,
		Fall,
		Land,
		Dodge,
		Hurt,
		Death,

		AttackNormal,
		AttackSpecial,
		Skill_1,
		Skill_2,
		Skill_4,
		Skill_3,

		Waiting = 998,	// 待機
		Ready = 999,	// 待機 (準備完了)
	};

	enum COLOR_PATTERN {
		DEFAULT,
		RED,
		GREEN,
		BLUE,
		END
	};
	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	void Render(const RenderContext& rc) override;

	// 外見パターンを読み取る
	void LoadAppearance(uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);

	void Jump();
	void InputMove(float elapsedTime);

	DirectX::XMFLOAT2 GetInputDirection();
	// 入力管理
	void UpdateInput();						// キー入力
	bool InputJump() { return (input & Input_Jump); }
	bool InputDodge();
	bool InputAttackNormal() { return (input & Input_Attack_N) > 0; }
	bool InputAttackSpecial() { return (input & Input_Attack_S) > 0; }
	bool InputSkill1() { return (input & Input_Skill_1) > 0; }
	bool InputSkill2() { return (input & Input_Skill_2) > 0; }
	bool InputSkill3() { return (input & Input_Skill_3) > 0; }
	bool InputSkill4() { return (input & Input_Skill_4) > 0; }
	bool ReleaseAttackNormal() { return (input & Input_R_Attack_N) > 0; }
	bool ReleaseAttackSpecial() { return (input & Input_R_Attack_S) > 0; }
	bool ReleaseSkill1() { return (input & Input_R_Skill_1) > 0; }
	bool ReleaseSkill2() { return (input & Input_R_Skill_2) > 0; }
	bool ReleaseSkill3() { return (input & Input_R_Skill_3) > 0; }
	bool ReleaseSkill4() { return (input & Input_R_Skill_4) > 0; }

	bool IsMove() { return velocity.x != 0.0f || velocity.z != 0.0f; }
	bool IsFall() { return velocity.y < -2.0f; }

	void FaceToCamera();
	void TurnByInput();

	uint64_t GetClientId() { return m_client_id; }
	void SetClientId(int id) { m_client_id = id; }
	int GetClassType() { return type; }

	float GetTurnSpeed() { return turnSpeed; }
	void SetTurnSpeed(float turnSpeed) { this->turnSpeed = turnSpeed; }

	void SetName(const char* name) { this->name = name; }

	void SetMenuVisibility(bool value) { this->m_menuVisible = value; }
	bool GetMenuVisibility() { return this->m_menuVisible; }

	void SetSaveFileName(std::string value) { this->m_SaveFile = value; }
	std::string GetCharacterSaveFileName() { return this->m_SaveFile; }

	// MP管理
	void RecoverMp(float elapsedTime);
	void ModifyMp(float mp);
	float GetMp() { return mp; }
	float GetMaxMp() { return maxMp; }

	// スキルタイマー
	float GetSkillTimerTime(int idx);
	float GetSkillTimerRate(int idx);
	void ResetSkillTimer(int idx);

	// スキル
	void SkillCost(int idx);

	void StopMove() { velocity.x = velocity.z = 0.0f; }
	void StopFall() { velocity.y = -gravity * T_TIMER.Delta() * 60.0f; }

	float GetMpCost(int idx);

	// 自機判定
	bool IsPlayer() { return GAME_DATA.GetClientId() == m_client_id; };

	// ダメージコールバック
	virtual void OnDamage(const HitResult& hit, int damage);

	// ターゲットを取得
	DirectX::XMFLOAT3 GetTarget() { return target; }
	// 発射物起点を取得
	virtual DirectX::XMFLOAT3 GetShotPosition() { return position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 }; }

	// ステートを取得
	int GetState() { return stateMachine->GetStateIndex(); }
	// ステートマシンを取得
	StateMachine<NonPlayerCharacter>* GetStateMachine() { return stateMachine; }

	Collider* GetAttackCollider(int idx) { return m_pattackColliders[idx]; }
	std::unordered_map<int, Collider*> GetAttackColliders() { return m_pattackColliders; }
	void EnableAttackColliders(bool enable = true) { for (const std::pair<int, Collider*>& collider : m_pattackColliders) collider.second->SetEnable(enable); }

	static DirectX::XMFLOAT4 GetColorSet(int idx) { return NonPlayerCharacter::colorSet[idx]; }
protected:
	void RegisterCommonState();
	void UpdateTarget();													// 自機用アイム目標更新
	virtual void UpdateColliders() override;								// 衝突判定の更新

	void UpdateSkillTimers(float elapsedTime);								// スキルタイマー

	bool CollisionVsEnemies(
		Collider* collider,
		int damage,
		bool power = false,
		float force = 0.0f,
		int effectIdx = -1,
		float effectScale = 1.0f
	); // 汎用 敵との判定

private:
	uint64_t m_client_id = 0;

	uint32_t input = 0;						// キー入力
	DirectX::XMFLOAT2 inputDirection = {};	// 移動方向
	DirectX::XMFLOAT3 target = {};			// アイム目標

	bool m_menuVisible = false;
	std::string m_SaveFile = "";

	float mp = 100.0f;
	float maxMp = 100.0f;
	float mpRecoverRate = 8.0f; // 毎秒回復量
	PLAYER_CLASS type = PLAYER_CLASS::Null;
	int atk = 10;
	std::string name;

	float moveSpeed = 0.0f;
	float turnSpeed = 0.0f;
	float jumpSpeed = 0.0f;
	float dodgeSpeed = 0.0f;

	// MP消費
	std::unordered_map<int, float> mpCost;

	// スキルクールタイム
	struct SkillTimer {
		float currentTimer;
		float time;
		SkillTimer(float time = 0.0f)
		{
			this->time = time;
			this->currentTimer = 0.0f;
		}
	};
	std::unordered_map<int, SkillTimer> skillTimer;
protected:

	static inline DirectX::XMFLOAT4 colorSet[COLOR_PATTERN::END] = {
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // DEFAULT
		{ 1.0f, 0.7f, 0.7f, 1.0f }, // RED
		{ 0.7f, 1.0f, 0.7f, 1.0f }, // GREEN,
		{ 0.7f, 0.7f, 1.0f, 1.0f }, // BLUE
	};

	StateMachine<NonPlayerCharacter>* stateMachine;

	std::unordered_map<int, Collider*> m_pattackColliders; // 攻撃判定
};