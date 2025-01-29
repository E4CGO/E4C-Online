#pragma once

#include "GameObject/Character/Character.h"
#include "GameObject/Character/Enemy/Enemy.h"

class Projectile : public ModelObject
{
public:
	Projectile(const char* filename, float scaling = 1.0f, Character* owner = nullptr) : ModelObject(filename, scaling, ModelObject::DX12), m_owner(owner) {}
	virtual ~Projectile() = default;

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;
	virtual void CollisionFunction(Collider* myCol, Collider* otherCol) {}

	void SetExistTime(float existTime) { m_existTime = existTime; }
	void SetCoolTime(float coolTime) { m_coolTime = coolTime; }
	void SetDirection(DirectX::XMFLOAT3 direction) { m_direction = direction; };
	void SetSpeed(float speed) { m_speed = speed; };
	void SetGravity(float gravity) { m_gravity = gravity; }
	Character* GetOwner() { return m_owner; }
	void SetOwner(Character* character) { m_owner = character; }
	void SetPower(uint16_t power) { m_power = power; }

protected:
	void Destory();

	virtual void OnDestory() {}
	//virtual void OnHitEnemy(HitResult& hit) {}
	//virtual void OnHitGround(HitResult& hit) { Destory(); }
	//virtual void OnHitWall(HitResult& hit) { Destory(); }

protected:
	float m_currentTimer = 0.0f;	// 現在時間
	float m_existTime = 0.0f;		// 存在時間
	float m_coolTime = 0.0f;			// 命中後無敵時間

	// 速力
	DirectX::XMFLOAT3 m_direction = { 0.0f, 0.0f, 0.0f };
	float m_speed = 0.0f;
	float m_gravity = 0.0f;

	Character* m_owner = nullptr;

	uint16_t m_power = 0;
};
