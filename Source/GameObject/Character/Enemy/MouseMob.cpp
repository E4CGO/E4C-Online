#include "MouseMob.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/Character/Enemy/MouseMobState.h"
#include "TAKOEngine/Physics/CollisionManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    scaling	スケール
	@param[in]    renderMode	レンダリング設定
*//***************************************************************************/
MouseMob::MouseMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMmouse_0117.glb", scaling, renderMode)
{
	// 敵の基本パラメーター
	enemyType = ENEMY_TYPE::MOUSE;
	radius = 0.6f;
	maxHp = hp = 50;
	atk = 10;
	moveSpeed = 2.0f;
	turnSpeed = DirectX::XMConvertToRadians(360);
	SetSearchRange(24.0f);
	//m_AttackRange = 1.25f;
	m_AttackRange = 1.8f;

	// 当たり判定
	//m_pColliders[HitCollider::BodyHit] = new SphereCollider(scaling * 1.2f);
	// 衝突判定
	SetMoveCollider({ { 0, radius / scale.y, 0 }, radius }, Collider::COLLIDER_OBJ::ENEMY);

	m_pColliders.clear();
	// ヒット判定
	Sphere sphere{ { 0, radius / scale.y, 0 }, radius };
	SetCollider(COLLIDER_ID::COL_BODY, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("JOT_C_Body")->worldTransform);
	{
		using namespace EnemyState::MouseMob;
		stateMachine->RegisterState(Enemy::STATE::IDLE, new IdleState(this));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(::Enemy::STATE::HURT, new HurtState(this));
		stateMachine->RegisterState(::Enemy::STATE::DEATH, new DeathState(this));

		stateMachine->RegisterState(STATE::WANDER, new MoveState(this));
		stateMachine->RegisterState(STATE::FOLLOW, new EnemyState::FollowState(this, 1.8f, STATE::ATTACK));

		stateMachine->RegisterState(STATE::ATTACK, new AttackState(this));
		stateMachine->RegisterState(STATE::ENCOUNTER, new EncounterState(this));
	}
}