#pragma once

#include "TAKOEngine/AI/StateMachine.h"
#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Physics/Collider.h"

#include "GameObject/ModelObject.h"

class Character : public ModelObject
{
public:
	Character(const char* filename, float scaling = 1.0f) : ModelObject(filename, scaling) {}
	virtual ~Character() = default;

	virtual void Update(float elapsedTime) override;
	void UpdateModel(float elapsedTime);
	virtual void Render(const RenderContext& rc) override;

	virtual bool MoveTo(float elapsedtime, const DirectX::XMFLOAT3& target, float moveSpeed, float turnSpeed);
	void Stop() { velocity = { 0.0f, 0.0f, 0.0f }; }

	void Turn(float elapsedTime, float vx, float vz, float speed);

	virtual void AttackCollision() {}

protected:
	// 移動
	void Move(float vx, float vz, float speed);
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

	virtual void OnLanding() {};
	virtual void OnWall() {};
public:
	// 影響
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

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

	const float GetMaxMoveSpeed() { return maxMoveSpeed; }

	float GetHeight() { return height; }

	// 地面判定
	bool IsGround() const { return isGround; }

	float GetHurtCoolTime() { return hurtCoolTime; }
	void SetHurtCoolTime(float second) { hurtCoolTime = second; }

	// HP
	int GetHp() { return hp; }
	int GetMaxHp() { return maxHp; }
	void ModifyHp(int hp);
protected:
	virtual void UpdateColliders() {};
protected:
	// レイキャスト用
	float stepOffset = 0.5f;
	float height = 2.0f;

	float gravity = -1.0f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	bool isGround = false;
	bool isWall = false;
	float friction = 0.5f;

	float acceleration = 100.0f;
	float maxMoveSpeed = 100.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.1f;

	float hurtCoolTime = 0.0f;

	// HP
	int hp = 1;
	int maxHp = 1;
};
