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
MouseMob::MouseMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDL_ENMmouth_1203.glb", scaling, renderMode)
{
	// 敵の基本パラメーター
	enemyType = ENEMY_TYPE::MOUSE;
	radius = 1.0f;
	maxHp = hp = 50;
	atk = 10;
	moveSpeed = 2.0f;
	turnSpeed = DirectX::XMConvertToRadians(180);
	m_SearchRange = 24.0f;
	m_AttackRange = 1.25f;

	// 当たり判定
	//m_pColliders[HitCollider::BodyHit] = new SphereCollider(scaling * 1.2f);
	// 衝突判定
	SetMoveCollider({ { 0, radius / scale.y, 0 }, radius }, Collider::COLLIDER_OBJ::ENEMY);

	m_pColliders.clear();
	// ヒット判定
	Sphere sphere{ { 0, radius / scale.y, 0 }, radius };
	SetCollider(COLLIDER::BODY, sphere, Collider::COLLIDER_OBJ::ENEMY, &transform);

	{
		using namespace enemy::mouseMob;
		// 基本ステート
		stateMachine->RegisterState(STATE::SEARCH, new SearchState(this));
		stateMachine->RegisterState(STATE::BATTLE, new BattleState(this));

		// 移動サブステート
		stateMachine->RegisterSubState(STATE::SEARCH, SEARCH_STATE::WANDER, new WanderState(this, 1.0f));
		stateMachine->RegisterSubState(STATE::SEARCH, SEARCH_STATE::IDLE, new IdleState(this, 3.0f, 5.0f));

		// 攻撃ステート
		stateMachine->RegisterSubState(STATE::BATTLE, BATTLE_STATE::PURSUIT, new PursuitState(this, 2.0f, m_AttackRange, 3.0f, 5.0f));
		stateMachine->RegisterSubState(STATE::BATTLE, BATTLE_STATE::ATTACK, new AttackState(this, 0.5f));

		stateMachine->SetState(STATE::SEARCH);
	}
}

// 一番近いプレイヤーをターゲット
void MouseMob::UpdateTarget()
{
	target = GetClosestPlayer(m_SearchRange);
}

//// 衝突判定更新
//void MouseMob::UpdateColliders()
//{
//	// 当たり判定
//	m_pColliders[COLL::BodyHit]->SetPosition(GetNodePosition("JOT_C_Body", DirectX::XMFLOAT3{ 0.0f, 50.0f, 0.0f } *scale));
//
//	// 攻撃判定
//	attackColliders[ATTACK_COLLIDER::BodyAtc]->SetPosition(GetNodePosition("JOT_C_Head", DirectX::XMFLOAT3{ 20.0f, 70.0f, 0.0f } *scale));
//}