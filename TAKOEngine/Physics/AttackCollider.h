//! @file AttackCollider.h
//! @note 

#ifndef __ATTACK_COLLIDER_H__
#define __ATTACK_COLLIDER_H__

#include "SphereCollider.h"
#include "CapsuleCollider.h"

class AttackSphereCollider : public SphereCollider
{
public:
	AttackSphereCollider(uint16_t _power, uint16_t _objType, DirectX::XMFLOAT4X4* _transform, float _hitStartRate = 0.0f, float _hitEndRate = 1.0f)
		: m_power(_power), SphereCollider(_objType, _transform), m_hitStartRate(_hitStartRate), m_hitEndRate(_hitEndRate) { m_enable = false; }
	~AttackSphereCollider() = default;

	virtual void Update() override;

	virtual void OnCollision(Collider* other) override;
	int GetPower() { return m_power; }
	
	void SetCurrentRate(float currentRate) override { m_currentRate = currentRate; }
	//const float GetCurrentRate() const override { return m_currentRate; }
	void SetHitStartRate(float rate) override { m_hitStartRate = rate; }
	const float GetHitStartRate() const override { return m_hitStartRate; }
	void SetHitEndRate(float rate) override { m_hitEndRate = rate; }
	const float GetHitEndRate() const override { return m_hitEndRate; }

protected:
	uint16_t m_power = 0;			// 攻撃力
	std::vector<GameObject*> m_hitOthers;	// 攻撃が当たった相手を保存して複数回当たらないようにする
	float m_currentRate = 0.0f;	// 現在のアニメーションレート
	float m_hitStartRate = 0.0f;	// 攻撃判定が発生するタイミングのアニメーションレート(min0%)
	float m_hitEndRate = 1.0f;	// 攻撃判定が消滅するタイミングのアニメーションレート(max100%)
};

class AttackCapsuleCollider : public CapsuleCollider
{
public:
	AttackCapsuleCollider(uint16_t _power, uint16_t _objType, DirectX::XMFLOAT4X4* _transform, float _hitStartRate = 0.0f, float _hitEndRate = 1.0f)
		: m_power(_power), CapsuleCollider(_objType, _transform), m_hitStartRate(_hitStartRate), m_hitEndRate(_hitEndRate) { m_enable = false; }
	~AttackCapsuleCollider() = default;

	void Update() override;

	virtual void OnCollision(Collider* other) override;
	int GetPower() { return m_power; }
	
	void SetCurrentRate(float currentRate) { m_currentRate = currentRate; }
	const float GetCurrentRate() const { return m_currentRate; }
	const float GetHitStartRate() const { return m_hitStartRate; }
	const float GetHitEndRate() const { return m_hitEndRate; }

protected:
	uint16_t m_power = 0;			// 攻撃力
	std::vector<GameObject*> m_hitOthers;	// 攻撃が当たった相手を保存して複数回当たらないようにする
	float m_currentRate = 0.0f;	// 現在のアニメーションレート
	float m_hitStartRate = 0.0f;	// 攻撃判定が発生するタイミングのアニメーションレート(min0%)
	float m_hitEndRate = 1.0f;	// 攻撃判定が消滅するタイミングのアニメーションレート(max100%)
};

#endif // !__ATTACK_COLLIDER_H__