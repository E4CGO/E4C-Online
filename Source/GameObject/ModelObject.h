#pragma once

#include "GameObject.h"

class ModelObject : public GameObject
{
public:
	ModelObject(const char* filename, float scaling = 1.0f);
	virtual ~ModelObject() = default;

	// XVˆ—
	virtual void Update(float elapsedTime) override;
	// •`‰æˆ—
	virtual void Render(const RenderContext& rc) override;

	std::unique_ptr<Model>& GetModel() { return model; }

	void Show() { visible = true; }
	void Hide() { visible = false; }

	void SetShader(ModelShaderId id);
	void SetAnimation(int index, bool loop, float blendSeconds = 0.2f);

	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	// Õ“Ë”»’è
	Collider* GetCollider() { return collider.get(); }
	void SetCollider(Collider::COLLIDER_TYPE collider);

	float GetAnimationSpeed() { return animationSpeed; }
	void SetAnimationSpeed(float speed) { animationSpeed = speed; }

	DirectX::XMFLOAT3 GetNodePosition(const char* nodeName, const DirectX::XMFLOAT3& offset = {});
	DirectX::XMFLOAT3 GetNodePosition(const DirectX::XMFLOAT3& offset = {});
protected:
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

	// Õ“Ë”»’è
	std::unique_ptr<Collider> collider = nullptr;

	ModelShaderId shaderId = ModelShaderId::Toon;
	std::unique_ptr<Model> model;

	bool visible = true;

	float animationSpeed = 1.0f;
};
