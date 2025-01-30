//! @file BearBoss.cpp
//! @note 

#include "BearBoss.h"
#include "BearBossState.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "Network/OnlineController.h"
#include "Scene/Stage/StageManager.h"

BearBoss::BearBoss(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMboss_0123.glb", scaling, renderMode)
{
	enemyType = ENEMY_TYPE::BEAR_BOSS;
	radius = 3.0f;
	maxHp = hp = 1000;
	atk = 10;
	moveSpeed = 1.0f;
	turnSpeed = DirectX::XMConvertToRadians(90);

	SetSearchRange(20.0f);

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
		stateMachine->RegisterState(Enemy::STATE::IDLE, new IdleState(this));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(::Enemy::STATE::HURT, nullptr);
		stateMachine->RegisterState(::Enemy::STATE::DEATH, new DeathState(this));

		stateMachine->RegisterState(STATE::WANDER, new MoveState(this));
		stateMachine->RegisterState(STATE::FOLLOW, new FollowState(this));
		stateMachine->RegisterState(STATE::ATTACK, new AttackState(this));

		stateMachine->RegisterState(STATE::ROAR, new RoarState(this));
		stateMachine->RegisterState(STATE::FORM_CHANGE, new FormChangeState(this));

		stateMachine->RegisterState(STATE::STUN, new StunState(this));
		stateMachine->RegisterSubState(STATE::STUN, STUN_STATE::STUN_START, new StunStartState(this));
		stateMachine->RegisterSubState(STATE::STUN, STUN_STATE::STUN_LOOP, new StunLoopState(this));
		stateMachine->RegisterSubState(STATE::STUN, STUN_STATE::STUN_END, new StunEndState(this));
	}
}


void BearBoss::OnDamage(const uint16_t& damage)
{
	if (IsMine() || ONLINE_CONTROLLER->GetState() != Online::State::SYNC)
	{
		hp -= damage;
		AddHate(PlayerCharacterManager::Instance().GetPlayerCharacterById()->GetClientId(), damage);
		if (hp > 0)
		{
			switch (m_phase)
			{
				case 0:
					if (this->hp < (this->maxHp / 4 * 3)) // HP 75%
					{
						EnemyState::StateTransition(this, STATE::STUN);
						m_phase++;
					}
					break;
				case 1:
					if (this->hp < (this->maxHp / 4 * 2)) // HP 50%
					{
						EnemyState::StateTransition(this, STATE::FORM_CHANGE);
						m_phase++;
					}
					break;
				case 2:
					if (this->hp < (this->maxHp / 4))  // HP 25%
					{
						EnemyState::StateTransition(this, STATE::STUN);
						m_phase++;
					}
					break;
			}
		}
		else
		{
			EnemyState::StateTransition(this, Enemy::STATE::DEATH);
		}
	}
	else
	{
		// 同期
		ONLINE_CONTROLLER->RegisterHit(enemy_id, damage);
	}
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

	if (time > existTime) {
		STAGES.GetStage()->DefeatBoss();
		Destory();
	}
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
