#pragma once
#include "Player.h"
class Rogue : public Player
{
public:
	Rogue();
	~Rogue() = default;

	DirectX::XMFLOAT3 GetShotPosition() override;

public:
	enum CHARGE_STATE {
		CHARGE_AIM,
		CHARGE_SHOOT
	};

	// ƒXƒLƒ‹1 (ŽèžÖ’e)
	enum SKILL_1_STATE {
		SKILL_1_AIM,
		SKILL_1_THROW
	};
};
