#pragma once

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Physics/Collider.h"

#include <DirectXMath.h>

// モデル用オブジェクト
class GameObject
{
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	void UpdateTransform();
	// 更新処理
	virtual void Update(float elapsedTime) = 0;
	// 描画処理
	virtual void Render(const RenderContext& rc) = 0;

	DirectX::XMFLOAT3 LocalPositiontoWorld(DirectX::XMFLOAT3 v);
	DirectX::XMFLOAT3 GetScreenPosition(
		const D3D11_VIEWPORT& viewport,
		const  DirectX::XMMATRIX& View,
		const  DirectX::XMMATRIX& Projection,
		const DirectX::XMMATRIX World
	);
	DirectX::XMFLOAT3 GetScreenPosition();

	virtual void DrawDebugPrimitive() {}
	virtual void DrawDebugGUI() {};
public:
	// アクセサ
	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// 位置設定
	virtual void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// 軸回転
	void RotateAxis(const DirectX::XMFLOAT3 Axis, float Angle);
	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	const DirectX::XMFLOAT3 GetFront() const { return { transform._31, transform._32, transform._33 }; }
	const DirectX::XMFLOAT3 GetRight() const { return { transform._11, transform._12, transform._13 }; }
	const DirectX::XMFLOAT3 GetUp() const { return { transform._21, transform._22, transform._23 }; }

protected:

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
};