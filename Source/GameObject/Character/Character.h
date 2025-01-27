//! @file Character.h
//! @note

#ifndef __INCLUDED_CHARACTER_OBJECT_H__
#define __INCLUDED_CHARACTER_OBJECT_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	Character
	@brief	キャラクタークラス
	@par	[説明]
*//***************************************************************************/
class Character : public ModelObject
{
public:
	// コンストラクタ
	Character() : ModelObject() {};
	// コンストラクタ（引数付き）
	Character(const char* filename, float scaling = 1.0f, ModelObject::RENDER_MODE renderMode = ModelObject::RENDER_MODE::DX11) : ModelObject(filename, scaling, renderMode) {}
	// デストラクタ
	virtual ~Character() = default;
	// 更新処理
	virtual void Update(float elapsedTime) override;

	// 移動処理
	virtual bool MoveTo(float elapsedtime, const DirectX::XMFLOAT3& target, float moveSpeed, float turnSpeed);

	// 速力初期化
	void Stop() { velocity = { 0.0f, 0.0f, 0.0f }; }

	// 向き
	void Turn(float elapsedTime, float vx, float vz, float speed);

	void FaceTo(const DirectX::XMFLOAT3 point)
	{
		angle.y = atan2(point.x - position.x, point.z - position.z);
		while (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
		while (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	}
	// 跳躍
	void Jump(float speed);
	// ベクトルを与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse)
	{
		velocity.x += impulse.x;
		velocity.y += impulse.y;
		velocity.z += impulse.z;
	}

	// 視界に存在
	bool InSight(const DirectX::XMFLOAT3 point, float angle = 60.0f);
protected:
	// 移動
	void Move(float vx, float vz, float speed);
	// 速力
	virtual void UpdateVelocity(float elapsedTime);
	// 垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);
	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);
	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);
	// 水平移動更新処理
	virtual void UpdateHorizontalMove(float elapsedTime);
	// 位置補正処理
	virtual void PositionAdjustment();

	// 着地コールバック
	virtual void OnLanding() {};
	// 壁衝突コールバック
	virtual void OnWall() {};
public:
	// アクセサ
	// 摩擦設定
	void SetFriction(const float friction) { this->friction = friction; }
	// 最大スピードを取得
	const float GetMaxMoveSpeed() { return maxMoveSpeed; }

	// 高さを取得
	float GetHeight() { return height; }
	// 半径を取得
	float GetRadius() { return radius; }

	// 地面判定
	bool IsGround() const { return isGround; }

	float GetHurtCoolTime() { return hurtCoolTime; }
	void SetHurtCoolTime(float second) { hurtCoolTime = second; }

	// HP
	int GetHp() { return hp; }
	int GetMaxHp() { return maxHp; }
	void ModifyHp(int hp);
	void SetCurrentHp(int hp) { this->hp = hp; }

	// 被ダメージコールバック
	virtual void OnDamage(const uint16_t& damage) { hp -= damage; }
	
	// スーパーアーマー
	bool GetSuperArmorFlag() { return superArmor; }
	void OnSuperArmor() { superArmor = true; }
	void OffSuperArmor() { superArmor = false; }

	//物理計算フラグ
	void SetKinematic(bool value) { this->isKinematic = value; }

	const float GetGravity() const { return gravity;  }
	void SetGravity(float gravity) { this->gravity = gravity; }
protected:
	// レイキャスト用
	float stepOffset = 0.5f;						// ステップ高さ
	float height = 2.0f;							// 高さ
	float radius = 0.5f;							// 半径

	float gravity = -1.0f;							// 重力
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };		// 速力
	bool isGround = false;							// 地面フラグ
	bool isWall = false;							// 壁フラグ
	float friction = 1.5f;

	// 移動用
	float acceleration = 100.0f;					// 加速度
	float maxMoveSpeed = 100.0f;					// 最大スピード
	float moveVecX = 0.0f;							// 移動量X
	float moveVecZ = 0.0f;							// 移動量Y
	float airControl = 0.1f;						// 空気抵抗

	float hurtCoolTime = 0.0f;						// 無敵時間
	bool superArmor = false;						// スーパーアーマーフラグ

	bool isKinematic = false;

	// HP
	int hp = 100;
	int maxHp = 100;
};
#endif