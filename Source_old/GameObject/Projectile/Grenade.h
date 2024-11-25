#pragma once
#include "Projectile.h"
class Grenade : public Projectile
{
public:
	Grenade(Character* owner);
	~Grenade() = default;

	void UpdateColliders() override;

	void OnDestory() override;
};
