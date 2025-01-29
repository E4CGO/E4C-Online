//! @file BearBoss.cpp
//! @note 

#include "BearBoss.h"
#include "BearBossState.h"
#include "TAKOEngine/Physics/CollisionManager.h"

BearBoss::BearBoss(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMboss_0123.glb", scaling, renderMode)
{
	enemyType = ENEMY_TYPE::BEAR_BOSS;
	radius = 3.0f;
	maxHp = hp = 500;
	atk = 10;
	moveSpeed = 1.0f;
	turnSpeed = DirectX::XMConvertToRadians(90);

	// 衝突判定
	SetMoveCollider({ { 0, radius, 0 }, radius }, Collider::COLLIDER_OBJ::ENEMY);
	m_pColliders.clear();
	// ヒット判定
	SetCollider(COLLIDER_ID::COL_BODY, { {0, 0, 0}, radius }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_C_Spine2")->worldTransform);
	uint16_t bodyArmor = 10;
	m_pColliders[COLLIDER_ID::COL_BODY]->SetArmor(bodyArmor);
	SetCollider(COLLIDER_ID::COL_HEAD, { {0, 1.8f / 0.05f, 0}, 1.8f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_C_Head")->worldTransform);
	SetCollider(COLLIDER_ID::COL_BUTTON, { {-(radius - 0.5f) / 0.03f, -0.5f / 0.03f, 0}, 1.0f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_C_Spine2")->worldTransform);
	SetCollider(COLLIDER_ID::COL_LEFT_LEG, { {0, 0, 0}, 2.0f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_L_Foot")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_LEFT_LEG]->SetArmor(bodyArmor);
	SetCollider(COLLIDER_ID::COL_RIGHT_LEG, { {0, 0, 0}, 2.0f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_R_Foot")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_RIGHT_LEG]->SetArmor(bodyArmor);
	SetCollider(COLLIDER_ID::COL_LEFT_HAND, { {0, 0, 0}, {1, 0, 0}, 2.0f, 1.4f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_L_Elbow")->worldTransform);
	SetCollider(COLLIDER_ID::COL_RIGHT_HAND, { {0, 0, 0}, {-1, 0, 0}, 2.0f, 1.4f }, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_R_Elbow")->worldTransform);

	OnSuperArmor();

	{
		using namespace EnemyState::BearBoss;
		stateMachine->RegisterState(Enemy::STATE::IDLE, new EnemyState::BearBoss::IdleState(this));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(::Enemy::STATE::HURT, nullptr);
		//stateMachine->RegisterState(::Enemy::STATE::DEATH, nullptr);

		stateMachine->RegisterState(STATE::WANDER, new EnemyState::BearBoss::MoveState(this));

		stateMachine->RegisterState(STATE::ATTACK, new AttackState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::FOLLOW, new AttackFollowState(this));
		stateMachine->RegisterSubState(STATE::ATTACK, ATTACK_STATE::PUNCH, new AttackPunchState(this));
	}
}

void BearBoss::UpdateTarget()
{
}

#include "TAKOEngine/Physics/AttackCollider.h"

PunchImpact::PunchImpact(DirectX::XMFLOAT3 pos, Character* owner) : Projectile("")
{
	SetOwner(owner);
	position = pos;

	MakeAttackCollider(20, 0, { pos, minRadius }, Collider::COLLIDER_OBJ::ENEMY_PROJECTILE, Collider::COLLIDER_OBJ::PLAYER, nullptr);
	m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });

}

void PunchImpact::Update(float elapsedTime)
{
	float radius = minRadius + (maxRadius - minRadius) * (time / existTime);
	m_pColliders[0]->SetParam({ position, radius });
	UpdateColliders();

	time += elapsedTime;

	if (time > existTime) Destory();
}

void PunchImpact::CollisionFunction(Collider* myCol, Collider* otherCol)
{
	AttackSphereCollider* attack = static_cast<AttackSphereCollider*>(myCol);
	for (GameObject* owner : attack->GetHitOthers())
	{
		// 既にヒットした敵には当たらない
		if (owner == otherCol->GetOwner()) return;
	}

	if (attack->GetPower() > otherCol->GetArmor())
	{
		Character* chara = static_cast<Character*>(otherCol->GetOwner());
		if (chara->IsGround())
		{
			uint16_t damage = attack->GetPower() - otherCol->GetArmor();
			attack->RegisterHitOthers(chara);
			chara->OnDamage(damage);
		}
	}
}
