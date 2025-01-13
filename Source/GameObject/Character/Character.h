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

	// 攻撃コリジョン
	virtual void AttackCollision() {}

	void FaceTo(const DirectX::XMFLOAT3 point)
	{
		angle.y = atan2(point.x - position.x, point.z - position.z);
		while (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
		while (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	}
protected:
	// 移動
	void Move(float vx, float vz, float speed);
	// 跳躍
	void Jump(float speed);

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
	// ベクトルを与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse)
	{
		velocity.x += impulse.x;
		velocity.y += impulse.y;
		velocity.z += impulse.z;
	}

	// アクセサ
	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	// 摩擦設定
	void SetFriction(const float friction) { this->friction = friction; }
	// 最大スピードを取得
	const float GetMaxMoveSpeed() { return maxMoveSpeed; }

	// 高さを取得
	float GetHeight() { return height; }

	// 地面判定
	bool IsGround() const { return isGround; }

	float GetHurtCoolTime() { return hurtCoolTime; }
	void SetHurtCoolTime(float second) { hurtCoolTime = second; }

	// HP
	int GetHp() { return hp; }
	int GetMaxHp() { return maxHp; }
	void ModifyHp(int hp);
	void SetCurrentHp(int hp) { this->hp = hp; }

	//物理計算フラグ
	void SetKinematic(bool value) { this->isKinematic = value; }

protected:
	// コライダー更新処理
	virtual void UpdateColliders() {};
protected:
	// レイキャスト用
	float stepOffset = 0.5f;						// ステップ高さ
	float height = 2.0f;							// 高さ

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

	bool isKinematic = false;

	// HP
	int hp = 100;
	int maxHp = 100;
};
#endif