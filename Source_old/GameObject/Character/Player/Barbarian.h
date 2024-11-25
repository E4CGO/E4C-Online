#pragma once

#include "Player.h"
#include "BarbarianState.h"

class Barbarian : public Player
{
public:
	Barbarian();
	~Barbarian() = default;

	void AttackCollision() override;
	DirectX::XMFLOAT3 GetShotPosition() override;
private:

	void UpdateColliders() override;
public:
	enum AttackCollider
	{
		Axe
	};

	// 一般攻撃（両手アックス）
	enum ATTACK_STATE {
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
	};

	// 特殊攻撃（投げアックス）
	enum THROW_STATE {
		THROW_AIM,
		THROW_ATTACK,
	};

	// スキル1 (回レ)
	enum SKILL_1_STATE {
		SKILL_1_START,
		SKILL_1_LOOP
	};
};
