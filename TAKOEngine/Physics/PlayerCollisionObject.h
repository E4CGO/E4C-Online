#pragma once

#include <memory>

#include "TAKOEngine/Physics/SphereCollider.h"

#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/Player.h"

class PlayerCollisionObject : public GameObject
{
public:
	PlayerCollisionObject(float radius = 1.0f);
	~PlayerCollisionObject() {};

	// �X�V����
	virtual void Update(float elapsedTime) override;
	// �`�揈��
	virtual void Render(const RenderContext& rc) override;

protected:
	virtual void OnHit(Player* player) {};
	virtual void OnLeave(Player* player) {};
protected:
	std::unique_ptr<Collider> collider = nullptr;
	bool isHit;
};
