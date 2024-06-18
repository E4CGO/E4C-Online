#pragma once

#include "TAKOEngine/AI/StateMachine.h"

#include <memory>
#include <unordered_map>

#include "GameObject/Character/Player/Player.h"

struct ENEMY_COLLISION
{
	int enemy_id = -1;
	int player_id = -1;
	int colider_id = -1;
	int count = -1;
	int damage = 0;
	DirectX::XMFLOAT3 force = {};
	bool power = false;
};

struct ENEMY_DATA
{
	int enemy_id = -1;
	int enemyType = -1;
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 velocity = {};
	int target = -1;
	DirectX::XMFLOAT3 angle = {};
	int state = -1;
	int subState = -1;
	int hp = 1;
	int maxHp = 1;
};

enum ENEMY_TYPE
{
	CHEST_COIN = 0,							// 宝箱
	CHEST_EMPTY = 1,						// 宝箱(空)

	CHEST_EMPTY_SKELETON_MINION,			// 宝箱(空) スケルトン生成

	SKELETON_MINION,						// デフォルト骨
	SKELETON_MINION_BOSS,					// デフォルト骨ボス
	END,
};

class Enemy : public Character
{
public:
	Enemy(const char* filename, float scaling = 1.0f);
	~Enemy();

	virtual void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;

	virtual void OnDamage(const ENEMY_COLLISION& hit);
	virtual void OnDeath();

	bool IsShowHp() { return showHp; }

	int GetAttack() { return atk; }

	void ImportData(ENEMY_DATA data);
	void ExportData(ENEMY_DATA& data);

	void SetEnemyId(int id) { enemy_id = id; }
	int GetEnemyId() { return enemy_id; }

	bool MoveTo(float elapsedTime, const DirectX::XMFLOAT3& target);
	void TurnTo(float elapsedTime, const DirectX::XMFLOAT3& target);

	Player* GetClosestPlayer(float limit = FLT_MAX);
	virtual void UpdateTarget() { target = nullptr; }
	void SetTarget(Player* player) { target = player; }
	Player* GetTarget() { return target; }

	int GetState() { return stateMachine->GetStateIndex(); }
	StateMachine<Enemy>* GetStateMachine() { return stateMachine; }

	std::unordered_map<int, Collider*> GetColliders() { return colliders; }
	std::unordered_map<int, Collider*> GetAttackColliders() { return attackColliders; }
	void EnableAttackColliders(bool enable = true) { for (const std::pair<int, Collider*>& collider : attackColliders) collider.second->SetEnable(enable); }
	virtual void AttackCollision() override;

	static Enemy* EnemyFactory(int enemyType);

public:
	enum Animation
	{
		Attack_1H,
		Attack_Combo,
		Attack_Spinning,
		Base,
		Block,
		Cheer,
		Climbing,
		Dance,
		Dash_Back,
		Dash_Front,
		Dash_Left,
		Dash_Right,
		Defeat,
		Heavy_Attack,
		Hop,
		Idle,
		Interact,
		Jump,
		Laying_Down_Idle,
		Pick_Up,
		Roll,
		Run,
		Shoot_1H,
		Shoot_2H,
		Shoot_Bow,
		Shooting_1H,
		Shooting_2H,
		Throw,
		Walk,
		Wave
	};
protected:
	int enemy_id = -1;
	int enemyType = -1;

	int atk = 10; // 攻撃力

	Player* target = nullptr;

	float moveSpeed = 0.0f;
	float turnSpeed = 0.0f;
	float jumpSpeed = 0.0f;

	StateMachine<Enemy>* stateMachine;

	int subState = -1;

	std::unordered_map<int, Collider*> colliders;		// 当たり判定
	std::unordered_map<int, Collider*> attackColliders;	// 攻撃判定

	bool showHp = true;	// HP表示
};
