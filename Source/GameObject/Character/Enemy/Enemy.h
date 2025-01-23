//! @file Enemy.h
//! @note

#ifndef __INCLUDED_ENEMY_H__
#define __INCLUDED_ENEMY_H__

#include "TAKOEngine/AI/StateMachine.h"

#include <memory>

#include "TAKOEngine/Tool/Mathf.h"

#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

struct ENEMY_COLLISION
{
	uint32_t enemy_id = -1;
	uint32_t player_id = -1;
	uint32_t colider_id = -1;
	int count = -1;
	int damage = 10;
	DirectX::XMFLOAT3 force = {};
	bool power = false;
};

enum ENEMY_TYPE : uint8_t
{
	MOUSE,									// ネズミ
	BEAR_BOSS,								// 熊ボス
	BIRD,									// 鳥
	PIG,									// 豚
	CROC,									// 鰐
	END,
};

class Spawner;

class Enemy : public Character
{
public:
	Enemy(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11);
	virtual ~Enemy();

public:
	enum STATE : uint8_t
	{
		IDLE,

		HURT,
		DEATH,

		END,
	};

#pragma pack(push, 1)
	struct SYNC_DATA
	{
		uint32_t enemy_id;
		uint8_t type;
		float position[3];
		uint32_t target_id;
		float targetPosition[3];
		float rotate;
		uint8_t state;
	};
#pragma pack(pop)

public:
	virtual void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;

	void SetSpawnPosition(const DirectX::XMFLOAT3& position) { this->m_SpawnPosition = position; }

	virtual void OnDamage(const uint16_t& damage) override;
	virtual void OnDeath();

	const bool IsShowHp() const { return showHp; }

	int GetAttack() { return atk; }

	void SetEnemyId(const uint32_t& id) { enemy_id = id; }
	const uint32_t GetEnemyId() const { return enemy_id; }

	bool IsAlive();

	bool MoveTo(float elapsedTime, const DirectX::XMFLOAT3& target);
	void TurnTo(float elapsedTime, const DirectX::XMFLOAT3& target);

	PlayerCharacter* GetClosestPlayer(float limit = FLT_MAX);
	virtual void UpdateTarget();

	int GetState() { return stateMachine->GetStateIndex(); }
	StateMachine<Enemy>* GetStateMachine() { return stateMachine; }

	static Enemy* EnemyFactory(uint8_t enemyType);

	void SetSpawner(Spawner* spawner) { m_pSpawner = spawner; }

	const bool IsMine() const { return m_isMine; }
	void SetIsMine(bool flag = true) { m_isMine = flag; }

	void SetRandomMoveTargetPosition();
	bool SearchPlayer();

	void SetSearchRange(const float range) { m_SearchRange = range; m_SearchRangeSq = range * range; }
	const float GetSearchRange() const { return m_SearchRange; }

	DirectX::XMFLOAT3 GetTargetPosition() { return m_TargetPosition; }
	void SetTargetPosition(const DirectX::XMFLOAT3 position) { m_TargetPosition = position; }

	void SetTarget(uint32_t client_id) { m_target = client_id; }
	void SetTarget(PlayerCharacter* player) { m_target = (player != nullptr) ? player->GetClientId() : UINT32_MAX ; }
	uint32_t GetTarget() { return m_target; }

	// 同期
	const SYNC_DATA SyncData ();
	void Sync(const SYNC_DATA& data);

	void AddHate(uint32_t client_id, int hate)
	{
		if (m_hate.contains(client_id))
		{
			m_hate[client_id] += hate;
		}
		else
		{
			m_hate[client_id] = hate;
		}
	}
	PlayerCharacter* GetHighestHateClient()
	{
		PlayerCharacter* result = nullptr;
		int highestHate = INT_MIN;
		for (const std::pair<uint32_t, int>client : m_hate)
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(client.first);
			if (player == nullptr)
			{
				m_hate.erase(client.first);
				continue;
			}
			if (highestHate < client.second)
			{
				result = player;
				highestHate = client.second;
			}
		}
		return result;
	}

protected:
	uint32_t enemy_id = 0;
	uint8_t enemyType = -1;

	int atk = 10; // 攻撃力

	uint32_t m_target = UINT32_MAX;
	DirectX::XMFLOAT3 m_TargetPosition = {};

	float moveSpeed = 0.0f;
	float turnSpeed = 0.0f;
	float jumpSpeed = 0.0f;

	DirectX::XMFLOAT3 m_SpawnPosition;
	float m_SearchRange;
	float m_SearchRangeSq;
	float m_AttackRange;

	StateMachine<Enemy>* stateMachine;

	int subState = -1;

	bool showHp = true;	// HP表示

	Spawner* m_pSpawner = nullptr; // スポナー

	bool m_isMine = false; // ローカル

	std::unordered_map<uint32_t, int> m_hate; //ヘイト値	
};

#endif //!__INCLUDED_ENEMY_H__