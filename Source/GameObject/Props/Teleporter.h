#pragma once

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Physics/Collider.h"

#include "GameObject/ModelObject.h"

class Teleporter : public ModelObject
{
public:
	Teleporter(const char* filename, float scaling = 1.0f) : ModelObject(filename, scaling) {}
	virtual ~Teleporter() = default;

	virtual void Update(float elapsedTime) override;
	void UpdateModel(float elapsedTime);
	virtual void Render(const RenderContext& rc) override;

	void CheckPlayer(DirectX::XMFLOAT3 playerCoords, float elapsedTime);

	bool GetPortalReady() { return isPortalReady; }
	void SetPortalReady() { this->isPortalReady = true; }

public:

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

	float GetHeight() { return height; }

protected:
	virtual void UpdateColliders() {};
protected:
	// レイキャスト用
	float stepOffset = 0.5f;
	float height = 2.0f;

	float portalTimer = 3.0f;
	bool isPortalReady = false;
};
