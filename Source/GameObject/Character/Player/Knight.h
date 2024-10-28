#pragma once
#include "Player.h"
class Knight : public Player
{
public:
	Knight();
	~Knight() = default;

	void AttackCollision() override;
	void RenderTrail();
	void OnDamage(const HitResult& hit, int damage) override;
	//���̋O�Ճm�[�h�擾
	const iModel::Node* GetSwordtrailNode();
private:
	void UpdateColliders() override;
	//�|���S���̍ő吔
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

	// ��ʍU���i���j
	enum ATTACK_STATE {
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
	};

	// ����U���i���j
	enum BLOCK_STATE {
		BLOCK_START,
		BLOCK_ATTACK,
		BLOCKING,
		BLOCK_HIT
	};
};
