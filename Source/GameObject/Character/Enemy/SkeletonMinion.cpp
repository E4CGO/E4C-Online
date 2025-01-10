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
	m_SearchRange = 24.0f;

	// 移動用Collider
	Sphere sphere({ 0, 0.6f / scaling, 0 }, 0.6f);
	SetMoveCollider(sphere, Collider::COLLIDER_OBJ::ENEMY);

	// 当たり判定
	SetCollider(COLLIDER_ID::COL_BODY, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_body")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_BODY]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);

	sphere.radius = 0.8f;
	SetCollider(COLLIDER_ID::COL_HEAD, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_head")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_HEAD]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER_ATTACK | Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);

	// 攻撃判定
	sphere.radius = 0.3f;
	sphere.position = { 0.25f, -0.45f, 0.0f };
	SetCollider(COLLIDER_ID::COL_LEFT_HAND, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_armLeft")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_LEFT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders[COLLIDER_ID::COL_LEFT_HAND]->SetEnable(false);

	sphere.position = { -0.25f, -0.45f, 0.0f };
	SetCollider(COLLIDER_ID::COL_RIGHT_HAND, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("character_skeleton_minion_armRight")->worldTransform);
	m_pColliders[COLLIDER_ID::COL_RIGHT_HAND]->SetHittableOBJ(Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders[COLLIDER_ID::COL_RIGHT_HAND]->SetEnable(false);

	stateMachine->RegisterState(Enemy::STATE::IDLE, new SkeletonMinionState::IdleState(this, 2.0f));
	stateMachine->RegisterState(SkeletonMinion::STATE::TARGET_FOUND, new EnemyState::FollowState(this, 2.0f, SkeletonMinion::STATE::ATTACK));
	stateMachine->RegisterState(SkeletonMinion::STATE::ATTACK, new SkeletonMinionState::AttackState(this));
	stateMachine->SetState(Enemy::STATE::IDLE);
}

SkeletonMinion::~SkeletonMinion()
{
	for (const std::pair<uint8_t, Collider*>& collider : m_pColliders)
	{
		COLLISIONS.Remove(collider.second);
	}
	m_pColliders.clear();
}

// 一番近いプレイヤーをターゲット
void SkeletonMinion::UpdateTarget()
{
	PlayerCharacter* player = GetClosestPlayer();
	m_target = (player == nullptr) ? UINT32_MAX : player->GetClientId();
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

	stateMachine->RegisterState(Enemy::STATE::IDLE, new EnemyState::IdleState(this, 1.0f));
	stateMachine->RegisterState(SkeletonMinion::STATE::TARGET_FOUND, new EnemyState::FollowState(this, 3.0f, SkeletonMinion::STATE::ATTACK));
	stateMachine->SetState(Enemy::STATE::IDLE);

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
	if (IsMine())
	{
		hp -= hit.damage;

		if (hp > 0)
		{
			if (armorHp <= 0) { // アーマーなし
				stateMachine->ChangeState(Enemy::STATE::HURT);
				hp -= hit.damage / 10 * 2;		// ダウン追加ダメージ
			}
			else if (hit.colider_id == COLLIDER_ID::COL_HEAD)	// ヘッドショット アーマーあり
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
			stateMachine->ChangeState(Enemy::STATE::DEATH);
		}
	}
}