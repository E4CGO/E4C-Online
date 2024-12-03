#include "SkeletonMinion.h"

#include "TAKOEngine/Physics/Collider/SphereCollider.h"
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

	// 当たり判定
	//colliders[HitCollider::Body] = new SphereCollider(scaling * 0.4f);
	//colliders[HitCollider::Head] = new SphereCollider(scaling * 0.5f);
	colliders[HitCollider::Body] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &transform);
	colliders[HitCollider::Head] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY, &transform);
	// 攻撃判定
	//attackColliders[AttackCollider::LeftHand] = new SphereCollider(scaling * 0.2f);
	//attackColliders[AttackCollider::RightHand] = new SphereCollider(scaling * 0.2f);
	attackColliders[AttackCollider::LeftHand] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &transform);
	attackColliders[AttackCollider::RightHand] = new SphereCollider(Collider::COLLIDER_OBJ::ENEMY_ATTACK, &transform);
	EnableAttackColliders(false);

	stateMachine->RegisterState(EnemyState::ID::TargetFound, new EnemyState::FollowState(this, 2.0f, SkeletonMinion::State::Attack));
	stateMachine->RegisterState(SkeletonMinion::State::Attack, new SkeletonMinionState::AttackState(this));
	stateMachine->SetState(EnemyState::Idle);
}

// 一番近いプレイヤーをターゲット
void SkeletonMinion::UpdateTarget()
{
	target = GetClosestPlayer(24.0f);
}

// 衝突判定更新
void SkeletonMinion::UpdateColliders()
{
	// 当たり判定
	//colliders[HitCollider::Body]->SetPosition(GetNodePosition("character_skeleton_minion_body", DirectX::XMFLOAT3{ 0.0f, 0.3f, 0.0f } *scale));
	//colliders[HitCollider::Head]->SetPosition(GetNodePosition("character_skeleton_minion_head", DirectX::XMFLOAT3{ 0.0f, 0.4f, 0.0f } *scale));

	// 攻撃判定
	//attackColliders[AttackCollider::LeftHand]->SetPosition(GetNodePosition("character_skeleton_minion_armLeft", DirectX::XMFLOAT3{ 0.25f, -0.45f, 0.0f } *scale));
	//attackColliders[AttackCollider::RightHand]->SetPosition(GetNodePosition("character_skeleton_minion_armRight", DirectX::XMFLOAT3{ -0.25f, -0.45f, 0.0f } *scale));
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

	stateMachine->RegisterState(EnemyState::ID::Idle, new EnemyState::IdleState(this, 1.0f));
	stateMachine->RegisterState(EnemyState::ID::TargetFound, new EnemyState::FollowState(this, 3.0f, SkeletonMinion::State::Attack));
	stateMachine->SetState(EnemyState::ID::Idle);

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
			stateMachine->ChangeState(EnemyState::ID::Hurt);
			hp -= hit.damage / 10 * 2;		// ダウン追加ダメージ
		}
		else if (hit.colider_id == HitCollider::Head)	// ヘッドショット アーマーあり
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
		stateMachine->ChangeState(EnemyState::ID::Death);
	}
}