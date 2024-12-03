//! @file Enemy.h
//! @note 
#ifndef __INCLUDED_ENEMY_H__
#define __INCLUDED_ENEMY_H__

#include "TAKOEngine/AI/StateMachine.h"

#include <memory>
#include <unordered_map>


#include "GameObject/Character/Player/PlayerCharacter.h"

struct ENEMY_COLLISION
{
	uint64_t enemy_id = -1;
	uint64_t player_id = -1;
	uint64_t colider_id = -1;
	int count = -1;
	int damage = 10;
	DirectX::XMFLOAT3 force = {};
	bool power = false;
};

enum ENEMY_TYPE : uint8_t
{
	SKELETON_MINION,						// デフォルト骨
	SKELETON_MINION_BOSS,					// デフォルト骨ボス
	END,
};

class Spawner;

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

	void SetEnemyId(const uint64_t& id) { enemy_id = id; }
	uint64_t GetEnemyId() { return enemy_id; }

	bool IsAlive();

	bool MoveTo(float elapsedTime, const DirectX::XMFLOAT3& target);
	void TurnTo(float elapsedTime, const DirectX::XMFLOAT3& target);

	PlayerCharacter* GetClosestPlayer(float limit = FLT_MAX);
	virtual void UpdateTarget() { target = nullptr; }
	void SetTarget(PlayerCharacter* player) { target = player; }
	PlayerCharacter* GetTarget() { return target; }

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
	uint64_t enemy_id = -1;
	uint8_t enemyType = -1;

	int atk = 10; // 攻撃力

	PlayerCharacter* target = nullptr;

	float moveSpeed = 0.0f;
	float turnSpeed = 0.0f;
	float jumpSpeed = 0.0f;

	StateMachine<Enemy>* stateMachine;

	int subState = -1;

	std::unordered_map<int, Collider*> colliders;		// 当たり判定
	std::unordered_map<int, Collider*> attackColliders;	// 攻撃判定

	bool showHp = true;	// HP表示

	Spawner* m_pSpawner = nullptr; // スポナー
};
#endif