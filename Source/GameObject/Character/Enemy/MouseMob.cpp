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
	m_SearchRange = 24.0f;
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
		using namespace EnemyState;
		// 基本ステート
		stateMachine->RegisterState(mouseMob::STATE::SEARCH, new mouseMob::SearchState(this));
		stateMachine->RegisterState(mouseMob::STATE::BATTLE, new mouseMob::BattleState(this));
		stateMachine->RegisterState(mouseMob::STATE::ENCOUNTER, new mouseMob::EncounterState(this));
		stateMachine->RegisterState(STATE::HURT, new mouseMob::HurtState(this));
		stateMachine->RegisterState(STATE::DEATH, new mouseMob::DeathState(this));

		// 移動サブステート
		stateMachine->RegisterSubState(mouseMob::STATE::SEARCH, mouseMob::SEARCH_STATE::WANDER, new mouseMob::WanderState(this, 1.0f));
		//stateMachine->RegisterSubState(mouseMob::STATE::SEARCH, mouseMob::SEARCH_STATE::IDLE, new mouseMob::IdleState(this, 3.0f, 5.0f));
		stateMachine->RegisterSubState(mouseMob::STATE::SEARCH, mouseMob::SEARCH_STATE::IDLE, new mouseMob::IdleState(this, 3.0f));

		// 攻撃ステート
		stateMachine->RegisterSubState(mouseMob::STATE::BATTLE, mouseMob::BATTLE_STATE::PURSUIT, new mouseMob::PursuitState(this, 0.5f, m_AttackRange, 3.0f, 5.0f));
		stateMachine->RegisterSubState(mouseMob::STATE::BATTLE, mouseMob::BATTLE_STATE::ATTACK, new mouseMob::AttackState(this, 0.5f));

		stateMachine->SetState(mouseMob::STATE::SEARCH);
	}
}

// 一番近いプレイヤーをターゲット
void MouseMob::UpdateTarget()
{
	PlayerCharacter* player = GetClosestPlayer(m_SearchRange);
	m_target = (player == nullptr) ? UINT32_MAX : player->GetClientId();
}