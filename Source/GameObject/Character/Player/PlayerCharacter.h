//! @file PlayerCharacter.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_H__
#define __INCLUDED_PLAYER_CHARACTER_H__

#include <unordered_map>
#include <mutex>

#include "TAKOEngine/AI/StateMachine.h"
#include "GameObject/Character/Character.h"
#include "GameData.h"
#include "PlayerCharacterData.h"

class Enemy;

static const uint32_t Input_Up = (1 << 0);			// 上
static const uint32_t Input_Down = (1 << 1);		// 下
static const uint32_t Input_Left = (1 << 2);		// 左
static const uint32_t Input_Right = (1 << 3);		// 右
static const uint32_t Input_Jump = (1 << 4);		// ジャンプ
static const uint32_t Input_Dodge = (1 << 5);		// 回避
static const uint32_t Input_Attack_N = (1 << 6);	// 一般攻撃
static const uint32_t Input_Attack_S = (1 << 7);	// 特殊攻撃
static const uint32_t Input_Skill_1 = (1 << 8);		// スキル1
static const uint32_t Input_Skill_2 = (1 << 9);		// スキル2
static const uint32_t Input_Skill_3 = (1 << 10);	// スキル3
static const uint32_t Input_Skill_4 = (1 << 11);	// スキル4
static const uint32_t Input_Menu = (1 << 12);		// メニュー

static const uint32_t Input_R_Attack_N = (1 << 13);	// 一般攻撃リリース
static const uint32_t Input_R_Attack_S = (1 << 14);	// 特殊攻撃リリース
static const uint32_t Input_R_Skill_1 = (1 << 15);	// スキル1リリース
static const uint32_t Input_R_Skill_2 = (1 << 16);	// スキル2リリース
static const uint32_t Input_R_Skill_3 = (1 << 17);	// スキル3リリース
static const uint32_t Input_R_Skill_4 = (1 << 18);	// スキル4リリース

class PlayerCharacter : public Character
{
public:
	// コンストラクタ(引数付き)
	PlayerCharacter(uint64_t id, const char* name, const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);
	// コンストラクタ(引数付き)
	PlayerCharacter(PlayerCharacterData::CharacterInfo dataInfo);
	// デストラクタ
	~PlayerCharacter();


#pragma pack(push, 1)
	// 同期用
	struct SYNC_DATA
	{
		uint64_t client_id;
		uint64_t sync_count_id;
		float position[3];
		float velocity[3];
		float rotate;
		uint8_t state;
		uint8_t sub_state;
	};
#pragma pack(pop)


	// KayKit Adventurers
	enum Animation
	{
		ANIM_IDLE,
		ANIM_MOVE_START,
		ANIM_MOVE,
		ANIM_ATTACK_SIMPLE,
	};

	enum STATE : uint8_t
	{
		IDLE,
		MOVE,
		JUMP,
		FALL,
		LAND,
		DODGE,
		HURT,
		DEATH,

		ATTACK_NORMAL,
		ATTACK_SPECIAL,
		SKILL_1,
		SKILL_2,
		SKILL_3,
		SKILL_4,

		WAITING = 254,	// 待機
		READY = 255,	// 待機 (準備完了)
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
	void RenderDX12(const RenderContextDX12& rc) override;

	// 外見パターンを読み取る
	void LoadAppearance(const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);

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
	bool IsFall() { return velocity.y < -10.0f; }

	void FaceToCamera();
	void TurnByInput();

	uint64_t GetClientId() { return m_client_id; }
	void SetClientId(const uint64_t id) { m_client_id = id; }

	float GetTurnSpeed() { return turnSpeed; }
	void SetTurnSpeed(float turnSpeed) { this->turnSpeed = turnSpeed; }

	const std::string& GetName() { return m_name; }
	void SetName(const char* name) { this->m_name = name; }

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
	StateMachine<PlayerCharacter>* GetStateMachine() { return stateMachine; }

	Collider* GetAttackCollider(int idx) { return m_pattackColliders[idx]; }
	std::unordered_map<int, Collider*> GetAttackColliders() { return m_pattackColliders; }
	void EnableAttackColliders(bool enable = true) { for (const std::pair<int, Collider*>& collider : m_pattackColliders) collider.second->SetEnable(enable); }


	// 同期用データを取得
	void GetSyncData(SYNC_DATA& data);
	// 同期用データを計算
	void ImportSyncData(const SYNC_DATA& data);

	static DirectX::XMFLOAT4 GetColorSet(int idx) { return PlayerCharacter::colorSet[idx]; }
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
	int atk = 10;
	std::string m_name;

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

	StateMachine<PlayerCharacter>* stateMachine;

	std::unordered_map<int, Collider*> m_pattackColliders; // 攻撃判定

	// 同期用
	std::mutex m_mut;
	// 同期補間
	struct TEMP_DATA {
		float timer = 0.0f;
		float time = 0.0f;
		DirectX::XMFLOAT3 position = {};
		float angle = 0.0f;
		uint64_t old_sync_count = 0;
		SYNC_DATA sync_data = {};
	} m_tempData;
};

#endif // __INCLUDED_PLAYER_CHARACTER_H__