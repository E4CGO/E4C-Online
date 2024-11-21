//! @file Collider.h
//! @note 

#ifndef __COLLIDER_H__
#define __COLLIDER_H__

// writer Matsumura
#include "TAKOEngine/Physics/Collision.h"

//struct HitResult
//{
//	DirectX::XMFLOAT3 position = { 0, 0, 0 };	// レイとポリゴンの交点
//	DirectX::XMFLOAT3 normal = { 0, 0, 0 };		// 衝突したポリゴンの法線ベクトル
//	DirectX::XMFLOAT3 rotation = { 0, 0, 0 };	// 回転量
//	float distance = FLT_MAX;						// レイの始点から交点までの距離
//	int materialIndex = -1;						// 衝突したポリゴンのマテリアル番号
//};

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
		UNROTATED_BOX,			// 回らないボックス
		CAPSULE,				// カプセル
		MODEL,					// モデル
		BOUNDING_BOX,			// バウンディングボックス
		MAP,					// マップ

		enum_count
	};
public:
	Collider() = default;
	virtual ~Collider() = default;

	bool Collision(
		Collider*& other,
		DirectX::XMFLOAT3 direction,
		HitResult& result
	);

	virtual bool CollisionVsUnrotatedBox(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsModel(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsShpere(
		Collider*& other,
		DirectX::XMFLOAT3& direction,
		HitResult& result
	) {
		return false;
	};

	virtual bool CollisionVsMap(bool wallCheck = false) 
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

	virtual DirectX::XMFLOAT3 GetTop() { return {}; };

	virtual void DrawDebugPrimitive(DirectX::XMFLOAT4 color = { 1, 1, 1, 1 }) {};
	virtual void DrawDebugGUI() {};

	void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }
	DirectX::XMFLOAT3 GetPosition() const { return position; };

	//void SetRotation(DirectX::XMFLOAT3 rot) { rotation = rot; }
	//DirectX::XMFLOAT3 GetRotation() const { return rotation; };

	void SetScale(DirectX::XMFLOAT3 s) { scale = s; }
	DirectX::XMFLOAT3 GetScale() const { return scale; }

	// Sphere用パラメータセット
	virtual void SetParam(Sphere sphere) {}
	// AABB用パラメータセット
	virtual void SetParam(AABB aabb) {}
	// Capsule用パラメータセット
	virtual void SetParam(Capsule capsule) {}

	COLLIDER_TYPE GetType() { return type; };

	bool IsEnable() { return enable; }
	void SetEnable(bool e) { enable = e; }
protected:
	COLLIDER_TYPE type = COLLIDER_TYPE::DEFAULT;

	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f }; // ワールド位置
	//DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f }; // ワールドローテーション
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

	bool enable = true;
};

#endif // !__COLLIDER_H__
