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

	// 移動用Collider
	SetMoveCollider(Collider::COLLIDER_TYPE::SPHERE, Collider::COLLIDER_OBJ::ENEMY);
	Sphere sphere({ 0, 0.6f / scaling, 0 }, 0.6f);
	m_pMoveCollider->SetParam(sphere);
	//collider->setTransform(&m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	m_pMoveCollider->SetOwner(this);

	// 当たり判定
	m_pColliders[COLLIDER::BODY] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	m_pColliders[COLLIDER::BODY]->SetParam(sphere);
	m_pColliders[COLLIDER::BODY]->SetOwner(this);
	m_pColliders[COLLIDER::BODY]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	COLLISIONS.Register(m_pColliders[COLLIDER::BODY]);

	m_pColliders[COLLIDER::HEAD] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_head")->worldTransform);
	sphere.radius = 0.8f;
	m_pColliders[COLLIDER::HEAD]->SetParam(sphere);
	m_pColliders[COLLIDER::HEAD]->SetOwner(this);
	m_pColliders[COLLIDER::HEAD]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	COLLISIONS.Register(m_pColliders[COLLIDER::HEAD]);

	// 攻撃判定
	m_pColliders[COLLIDER::LEFT_HAND] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &m_pmodels[0]->FindNode("character_skeleton_minion_armLeft")->worldTransform);
	sphere.radius = 0.3f;
	sphere.position = { 0.25f, -0.45f, 0.0f };
	m_pColliders[COLLIDER::LEFT_HAND]->SetParam(sphere);
	m_pColliders[COLLIDER::LEFT_HAND]->SetOwner(this);
	m_pColliders[COLLIDER::LEFT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	COLLISIONS.Register(m_pColliders[COLLIDER::LEFT_HAND]);

	m_pColliders[COLLIDER::RIGHT_HAND] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &m_pmodels[0]->FindNode("character_skeleton_minion_armRight")->worldTransform);
	sphere.position = { -0.25f, -0.45f, 0.0f };
	m_pColliders[COLLIDER::RIGHT_HAND]->SetParam(sphere);
	m_pColliders[COLLIDER::RIGHT_HAND]->SetOwner(this);
	m_pColliders[COLLIDER::RIGHT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	COLLISIONS.Register(m_pColliders[COLLIDER::RIGHT_HAND]);

	stateMachine->RegisterState(enemy::STATE::TARGET_FOUND, new enemy::FollowState(this, 2.0f, SkeletonMinion::STATE::ATTACK));
	stateMachine->RegisterState(SkeletonMinion::STATE::ATTACK, new SkeletonMinionState::AttackState(this));
	stateMachine->SetState(enemy::STATE::IDLE);
}

// 一番近いプレイヤーをターゲット
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

	// スーパーアーマー
	armorMaxHp = armorHp = 50;

	stateMachine->RegisterState(enemy::STATE::IDLE, new enemy::IdleState(this, 1.0f));
	stateMachine->RegisterState(enemy::STATE::TARGET_FOUND, new enemy::FollowState(this, 3.0f, SkeletonMinion::STATE::ATTACK));
	stateMachine->SetState(enemy::STATE::IDLE);

	// HPゲージ
	UI.Register(new WidgetBossHp("スケルドン", this));
}
void SkeletonMinionBoss::Update(float elaspedTime)
{
	if (armorHp <= 0) // アーマーブレイク
	{
		recoverArmorTimer -= elaspedTime;
		if (recoverArmorTimer <= 0.0f)
		{
			recoverArmorTimer = 0.0f;
			armorHp = armorMaxHp;		// アーマー回復
		}
	}

	Enemy::Update(elaspedTime);


}
void SkeletonMinionBoss::OnDamage(const ENEMY_COLLISION& hit)
{
	hp -= hit.damage;

	if (hp > 0)
	{
		if (armorHp <= 0) { // アーマーなし
			stateMachine->ChangeState(enemy::STATE::HURT);
			hp -= hit.damage / 10 * 2;		// ダウン追加ダメージ
		}
		else if (hit.colider_id == COLLIDER::HEAD)	// ヘッドショット アーマーあり
		{
			armorHp -= hit.damage;
			if (armorHp <= 0) {		// アーマー解除
				armorHp = 0;
				recoverArmorTimer = recoverArmorTime;	// アーマー回復タイマー
			}
		}
		velocity += hit.force * 0.1f;
	}

	if (hp <= 0)
	{
		stateMachine->ChangeState(enemy::STATE::DEATH);
	}
}