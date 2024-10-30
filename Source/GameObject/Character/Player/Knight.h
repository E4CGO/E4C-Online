#pragma once
#include "Player.h"
class Knight : public Player
{
public:
	Knight();
	~Knight() = default;

	void AttackCollision() override;
	void OnDamage(const HitResult& hit, int damage) override;
private:
	void UpdateColliders() override;
	//ポリゴンの最大数
	static const int MAX_POLYGON = 12;
	XMFLOAT3 trailPosition[2][MAX_POLYGON];
public:
	enum AttackCollider
	{
		Sword_1,
		Sword_2,
		Sword_3,
		Shield
	};

	// 一般攻撃（剣）
	enum ATTACK_STATE {
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
	};

	// 特殊攻撃（盾）
	enum BLOCK_STATE {
		BLOCK_START,
		BLOCK_ATTACK,
		BLOCKING,
		BLOCK_HIT
	};
};
