#include "SkeletonMinion.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "UI/Widget/WidgetBossHp.h"

#include "GameObject/Character/Enemy/SkeletonMinionState.h"

SkeletonMinion::SkeletonMinion(float scaling) : Enemy("Data/Model/Enemy/character_skeleton_minion.gltf", scaling)
{
	enemyType = ENEMY_TYPE::SKELETON_MINION;
	radius = 0.6f;
	maxHp = hp = 50;
	atk = 10;
	moveSpeed = 2.0f;
	turnSpeed = DirectX::XMConvertToRadians(180);

	// �ړ��pCollider
	Sphere sphere({ 0, 0.6f / scaling, 0 }, 0.6f);
	SetMoveCollider(sphere, Collider::COLLIDER_OBJ::ENEMY);

	// �����蔻��
	SetCollider(COLLIDER_ID::COL_BODY, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_BODY]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);

	sphere.radius = 0.8f;
	SetCollider(COLLIDER_ID::COL_HEAD, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_head")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_HEAD]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	
	// �U������
	sphere.radius = 0.3f;
	sphere.position = { 0.25f, -0.45f, 0.0f };
	SetCollider(COLLIDER_ID::COL_LEFT_HAND, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_armLeft")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_LEFT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders[COLLIDER_ID::COL_LEFT_HAND]->SetEnable(false);

	sphere.position = { -0.25f, -0.45f, 0.0f };
	SetCollider(COLLIDER_ID::COL_RIGHT_HAND, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_armRight")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_RIGHT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders[COLLIDER_ID::COL_RIGHT_HAND]->SetEnable(false);


	stateMachine->RegisterState(EnemyState::ID::TargetFound, new EnemyState::FollowState(this, 2.0f, SkeletonMinion::State::Attack));
	stateMachine->RegisterState(SkeletonMinion::State::Attack, new SkeletonMinionState::AttackState(this));
	stateMachine->SetState(EnemyState::Idle);
}

// ��ԋ߂��v���C���[���^�[�Q�b�g
void SkeletonMinion::UpdateTarget()
{
	target = GetClosestPlayer(24.0f);
}

SkeletonMinionBoss::SkeletonMinionBoss() : SkeletonMinion(3.0f)
{
	enemyType = ENEMY_TYPE::SKELETON_MINION_BOSS;
	maxHp *= 10;
	hp = maxHp;
	atk = 20;
	moveSpeed = 3.0f;
	turnSpeed = DirectX::XMConvertToRadians(90);

	showHp = true;

	// �X�[�p�[�A�[�}�[
	armorMaxHp = armorHp = 50;

	stateMachine->RegisterState(EnemyState::ID::Idle, new EnemyState::IdleState(this, 1.0f));
	stateMachine->RegisterState(EnemyState::ID::TargetFound, new EnemyState::FollowState(this, 3.0f, SkeletonMinion::State::Attack));
	stateMachine->SetState(EnemyState::ID::Idle);

	// HP�Q�[�W
	UI.Register(new WidgetBossHp("�X�P���h��", this));
}
void SkeletonMinionBoss::Update(float elaspedTime)
{
	if (armorHp <= 0) // �A�[�}�[�u���C�N
	{
		recoverArmorTimer -= elaspedTime;
		if (recoverArmorTimer <= 0.0f)
		{
			recoverArmorTimer = 0.0f;
			armorHp = armorMaxHp;		// �A�[�}�[��
		}
	}

	Enemy::Update(elaspedTime);

	
}
void SkeletonMinionBoss::OnDamage(const ENEMY_COLLISION& hit)
{
	hp -= hit.damage;

	if (hp > 0)
	{
		if (armorHp <= 0) { // �A�[�}�[�Ȃ�
			stateMachine->ChangeState(EnemyState::ID::Hurt);
			hp -= hit.damage / 10 * 2;		// �_�E���ǉ��_���[�W
		}
		else if (hit.colider_id == HitCollider::Head)	// �w�b�h�V���b�g �A�[�}�[����
		{
			armorHp -= hit.damage;
			if (armorHp <= 0) {		// �A�[�}�[����
				armorHp = 0;
				recoverArmorTimer = recoverArmorTime;	// �A�[�}�[�񕜃^�C�}�[
			}
		}
		velocity += hit.force * 0.1f;
	}

	if (hp <= 0)
	{
		stateMachine->ChangeState(EnemyState::ID::Death);
	}
}