//! @file Collider.h
//! @note 

#ifndef __COLLIDER_H__
#define __COLLIDER_H__

// writer Matsumura
#include "TAKOEngine/Physics/Collision.h"

// 継承Collider前方宣言
class SphereCollider;
class AABBCollider;
class OBBCollider;
class CapsuleCollider;
class ModelCollider;

/**************************************************************************//**
	@class	Collider
	@brief	当たり判定用コリダー基底クラス
	@par	[説明]
*//***************************************************************************/
class Collider
{
public:
	enum class COLLIDER_TYPE
	{
		DEFAULT,
		SPHERE,					// 球体
		AABB,					// 不回転ボックス
		OBB,					// 回転可能ボックス
		CAPSULE,				// カプセル
		MODEL,					// モデル
		MAP,					// マップ

		enum_count
	};

	enum COLLIDER_OBJ : uint16_t
	{
		PLAYER				= 1 << 0,
		PLAYER_ATTACK		= 1 << 1,
		PLAYER_BLOCK		= 1 << 2,
		PLAYER_PROJECTILE	= 1 << 3,
		ENEMY				= 1 << 4,
		ENEMY_ATTACK		= 1 << 5,
		ENEMY_PROJECTILE	= 1 << 6,
		ITEM				= 1 << 7,
		OBSTRUCTION			= 1 << 8,

		HIT_ERR = 1 << 9	// この値以上はエラー
	};

public:
	Collider() = default;
	Collider(uint16_t objType, DirectX::XMFLOAT4X4* transform)
	{
		m_OBJType = objType;
		m_pTransform = transform;
	}
	virtual ~Collider() = default;

	bool Collision(
		Collider* other,
		DirectX::XMFLOAT3 direction,
		HitResult& result
	);

	virtual bool CollisionVsShpere(
		SphereCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsAABB(
		AABBCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsOBB(
		OBBCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsCapsule(
		CapsuleCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsModel(
		ModelCollider* other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsMap()
	{
		return false;
	};

	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		HitResult& result
	) {
		return false;
	}

	void Update();

	//virtual DirectX::XMFLOAT3 GetTop() { return {}; };

	virtual void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) {};
	virtual void DrawDebugGUI() {};

	// Sphere用パラメータセット
	virtual void SetParam(Sphere sphere) {}
	// Sphere用パラメータゲット
	virtual Sphere GetSphere() { return Sphere{}; }
	// AABB用パラメータセット
	virtual void SetParam(AABB aabb) {}
	// AABB用パラメータゲット
	virtual AABB GetAABB() { return AABB{}; }
	// OBB用パラメータセット
	virtual void SetParam(OBB obb) {}
	// OBB用パラメータゲット
	virtual OBB GetOBB() { return OBB{}; }
	// Capsule用パラメータセット
	virtual void SetParam(Capsule capsule) {}
	// Capsule用パラメータゲット
	virtual Capsule GetCapsule() { return Capsule{}; }

	const COLLIDER_TYPE GetType() const { return m_shapeType; };

	const uint16_t GetOBJType() const { return m_OBJType; }

	//void SetPosition(const DirectX::XMFLOAT3 pos) { m_position = pos; }
	const DirectX::XMFLOAT3 GetPosition() const { return m_position; }

	void SetHittableOBJ(uint16_t hit) { m_hittableOBJType = hit; }
	const uint16_t GetHittableOBJ() const { return m_hittableOBJType; }

	void SetHitStartRate(float rate) { m_hitStartRate = rate; }
	const float GetHitStartRate() const { return m_hitStartRate; }
	void SetHitEndRate(float rate) { m_hitEndRate = rate; }
	const float GetHitEndRate() const { return m_hitEndRate; }

	bool IsEnable() const { return m_enable; }
	void SetEnable(bool e) { m_enable = e; }

protected:
	COLLIDER_TYPE m_shapeType = COLLIDER_TYPE::DEFAULT;
	uint16_t m_OBJType = 0;
	uint16_t m_hittableOBJType = 0;
	float m_hitStartRate = 0.0f;	// 攻撃判定が発生するタイミングのアニメーションレート(min0%)
	float m_hitEndRate = 1.0f;	// 攻撃判定が消滅するタイミングのアニメーションレート(max100%)
	bool m_enable = true;

	DirectX::XMFLOAT4X4* m_pTransform = nullptr;			// ワールド行列
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };	// ワールド位置
	DirectX::XMFLOAT3 m_prePosition = { 0.0f, 0.0f, 0.0f };	// 前フレームのワールド位置
	DirectX::XMFLOAT3 m_offset = { 0.0f, 0.0f, 0.0f };		// ローカル空間での補正位置
};

#endif // !__COLLIDER_H__
