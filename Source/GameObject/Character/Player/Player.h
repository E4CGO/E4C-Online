//! @file Player.h
//! @note 


#ifndef __INCLUDED_PLAYER_H__
#define __INCLUDED_PLAYER_H__


#include "TAKOEngine/AI/StateMachine.h"

#include <memory>
#include <unordered_map>

#include "GameObject/Character/Character.h"
#include "GameData.h"

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

enum PLAYER_CLASS
{
	Null = -1,
	KNIGHT = 0,
	BARBARIAN,
	ROGUE,

	End,
	MAGE,

	CUSTOMIZE
};

struct PLAYER_DATA
{
	uint64_t client_id = 0;
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 velocity = {};
	DirectX::XMFLOAT3 target = {};
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float angle = 0.0f;
	int state = -1;
	int subState = -1;
	int hp = 1;
	int maxHp = 1;
	bool connected = true;
};

/**************************************************************************//**
	@class	Player
	@brief	プレイヤーキャラクタークラス
	@par	[説明]
			入力処理付き
*//***************************************************************************/
class Player : public Character
{
public:
	// コンストラクタ
	Player() : Character() {};
	// コンストラクタ(引数付き)
	Player(const char* filename, float scaling = 1.0f);
	// デストラクタ
	~Player();

	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 描画処理
	void Render(const RenderContext& rc) override;

	// 跳躍
	void Jump();

	// 移動入力処理
	void InputMove(float elapsedTime);

	// 入力方向を取得
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

	// 移動判定
	bool IsMove() { return velocity.x != 0.0f || velocity.z != 0.0f; }
	// 落下判定
	bool IsFall() { return velocity.y < -2.0f; }

	// カメラ―に向ける
	void FaceToCamera();
	// 入力向高に向ける
	void TurnByInput();

	// インポートデータ （ネットワーク用）
	void ImportData(PLAYER_DATA data);
	// エクスポートデータ （ネットワーク用）
	void ExportData(PLAYER_DATA& data);

	// クライアントIDを取得
	int GetClientId() { return client_id; }
	// クライアントIDを設定
	void SetClientId(int id) { client_id = id; }
	// クラスタイプを取得
	int GetClassType() { return type; }

	// 回転スピードを取得
	float GetTurnSpeed() { return turnSpeed; }
	// 回転スピードを設定
	void SetTurnSpeed(float turnSpeed) { this->turnSpeed = turnSpeed; }
	// ネームドを設定
	void SetName(const char* name) { strcpy_s(this->name, 64, name); };

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

	// 水平移動を停止
	void StopMove() { velocity.x = velocity.z = 0.0f; }
	// 落下移動を停止
	void StopFall() { velocity.y = -gravity * T_TIMER.Delta() * 60.0f; }

	float GetMpCost(int idx);

	// 自機判定
	bool IsPlayer() { return GAME_DATA.GetClientId() == client_id; };

	// ダメージコールバック
	virtual void OnDamage(const HitResult& hit, int damage);

	// ターゲットを取得
	DirectX::XMFLOAT3 GetTarget() { return target; }
	// 発射位置
	virtual DirectX::XMFLOAT3 GetShotPosition() { return position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 }; }

	// ステートを取得
	int GetState() { return stateMachine->GetStateIndex(); }

	// ステートマシンを取得
	StateMachine<Player>* GetStateMachine() { return stateMachine; }

	// 攻撃コライダーを取得
	Collider* GetAttackCollider(int idx) { return attackColliders[idx]; }
	// 攻撃コライダーリsづとを取得
	std::unordered_map<int, Collider*>& GetAttackColliders() { return attackColliders; }
	// 攻撃コライダーの有効・無効化
	void EnableAttackColliders(bool enable = true) { for (const std::pair<int, Collider*>& collider : attackColliders) collider.second->SetEnable(enable); }
	// 色セットを取得
	static DirectX::XMFLOAT4 GetColorSet(int idx) { return Player::colorSet[idx]; }
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
public:
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
protected:
	uint32_t input = 0;						// キー入力
	DirectX::XMFLOAT2 inputDirection = {};	// 移動方向
	DirectX::XMFLOAT3 target = {};			// アイム目標

	int client_id = -1;
	float mp = 100.0f;
	float maxMp = 100.0f;
	float mpRecoverRate = 8.0f; // 毎秒回復量
	PLAYER_CLASS type = PLAYER_CLASS::Null;
	int atk = 10;
	char name[64];

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

	StateMachine<Player>* stateMachine;

	std::unordered_map<int, Collider*> attackColliders; // 攻撃判定
};
#endif