#include "SkeletonMinion.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "UI/Widget/WidgetBossHp.h"

#include "GameObject/Character/Enemy/SkeletonMinionState.h"

SkeletonMinion::SkeletonMinion(float scaling) : Enemy("Data/Model/Enemy/character_skeleton_minion.gltf", scaling)
{
	enemyType = ENEMY_TYPE::SKELETON_MINION;
	maxHp = hp = 50;
	atk = 10;
	moveSpeed = 2.0f;
	turnSpeed = DirectX::XMConvertToRadians(180);

	// �ړ��pCollider
	SetCollider(Collider::COLLIDER_TYPE::SPHERE, Collider::COLLIDER_OBJ::ENEMY);
	Sphere sphere({ 0, 0.6f / scaling, 0 }, 0.6f);
	collider->SetParam(sphere);
	//collider->setTransform(&m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	collider->SetOwner(this);

	// �����蔻��
	colliders[HitCollider::Body] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	colliders[HitCollider::Body]->SetParam(sphere);
	colliders[HitCollider::Body]->SetOwner(this);
	colliders[HitCollider::Body]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	COLLISIONS.Register(colliders[HitCollider::Body]);

	colliders[HitCollider::Head] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_head")->worldTransform);
	sphere.radius = 0.8f;
	colliders[HitCollider::Head]->SetParam(sphere);
	colliders[HitCollider::Head]->SetOwner(this);
	colliders[HitCollider::Head]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	COLLISIONS.Register(colliders[HitCollider::Head]);
	
	// �U������
	attackColliders[AttackCollider::LeftHand] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &m_pmodels[0]->FindNode("character_skeleton_minion_armLeft")->worldTransform);
	sphere.radius = 0.3f;
	sphere.position = { 0.25f, -0.45f, 0.0f };
	attackColliders[AttackCollider::LeftHand]->SetParam(sphere);
	attackColliders[AttackCollider::LeftHand]->SetOwner(this);
	attackColliders[AttackCollider::LeftHand]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	COLLISIONS.Register(attackColliders[AttackCollider::LeftHand]);

	attackColliders[AttackCollider::RightHand] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &m_pmodels[0]->FindNode("character_skeleton_minion_armRight")->worldTransform);
	sphere.position = { -0.25f, -0.45f, 0.0f };
	attackColliders[AttackCollider::RightHand]->SetParam(sphere);
	attackColliders[AttackCollider::RightHand]->SetOwner(this);
	attackColliders[AttackCollider::RightHand]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	COLLISIONS.Register(attackColliders[AttackCollider::RightHand]);

	EnableAttackColliders(false);

	stateMachine->RegisterState(EnemyState::ID::TargetFound, new EnemyState::FollowState(this, 2.0f, SkeletonMinion::State::Attack));
	stateMachine->RegisterState(SkeletonMinion::State::Attack, new SkeletonMinionState::AttackState(this));
	stateMachine->SetState(EnemyState::Idle);
}

// ��ԋ߂��v���C���[���^�[�Q�b�g
void SkeletonMinion::UpdateTarget()
{
	target = GetClosestPlayer(24.0f);
}

// �Փ˔���X�V
void SkeletonMinion::UpdateColliders()
{
	if (collider)
	{
		collider->Update();
		for (const std::pair<int, Collider*>& attackCollider : attackColliders)
		{
			attackCollider.second->Update();
		}
		for (const std::pair<int, Collider*>& hitCollider : colliders)
		{
			hitCollider.second->Update();
		}
	}
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