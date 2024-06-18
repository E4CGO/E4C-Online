#pragma once
#include "Projectile.h"
class TwoHandedAxe : public Projectile
{
public:
	TwoHandedAxe(Character* owner);
	~TwoHandedAxe() = default;

	void Update(float elapsedTime) override;
	void UpdateColliders() override;

	void DrawDebugPrimitive() override;

protected:
	void OnHitWall(HitResult& hit) override;
	void OnHitEnemy(HitResult& hit) override;
private:
	float timer = 2.0f; // Žõ–½
	float rotateAngle = DirectX::XMConvertToRadians(720);
};
