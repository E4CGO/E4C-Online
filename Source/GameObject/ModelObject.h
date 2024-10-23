#pragma once

#include "GameObject.h"

#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/NewModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"

class ModelObject : public GameObject
{
public:
	ModelObject();
	ModelObject(const char* filename, float scaling = 1.0f, std::string renderMode = "DX11");
	virtual ~ModelObject() = default;

	// 更新処理
	virtual void Update(float elapsedTime) override;
	// 描画処理
	virtual void Render(const RenderContext& rc) override;

	std::unique_ptr<iModel>& GetModel() { return model; }

	void Show() { visible = true; }
	void Hide() { visible = false; }

	void SetShader(ModelShaderId id);
	void SetAnimation(int index, bool loop, float blendSeconds = 0.2f);

	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	// 衝突判定
	Collider* GetCollider() { return collider.get(); }
	void SetCollider(Collider::COLLIDER_TYPE collider);

	float GetAnimationSpeed() { return animationSpeed; }
	void SetAnimationSpeed(float speed) { animationSpeed = speed; }

	DirectX::XMFLOAT3 GetNodePosition(const char* nodeName, const DirectX::XMFLOAT3& offset = {});
	DirectX::XMFLOAT3 GetNodePosition(const DirectX::XMFLOAT3& offset = {});
protected:
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

	// 衝突判定
	std::unique_ptr<Collider> collider = nullptr;

	ModelShaderId shaderId = ModelShaderId::Toon;
	std::unique_ptr<iModel> model;

	bool visible = true;

	float animationSpeed = 1.0f;
};
