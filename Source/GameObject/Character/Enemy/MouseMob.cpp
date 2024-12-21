#include "MouseMob.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/Character/Enemy/MouseMobState.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    scaling	スケール
	@param[in]    renderMode	レンダリング設定
*//***************************************************************************/
MouseMob::MouseMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDL_ENMmouth_1203.glb", scaling, renderMode)
{
	// 敵の基本パラメーター
	enemyType = ENEMY_TYPE::MOUSE;
	maxHp = hp = 50;
	atk = 10;
	moveSpeed = 2.0f;
	turnSpeed = DirectX::XMConvertToRadians(180);
	m_SearchRange = 24.0f;
	m_AttackRange = 1.25f;

	// 当たり判定
	colliders[HitCollider::BodyHit] = new SphereCollider(scaling * 1.2f);
	// 攻撃判定
	attackColliders[AttackCollider::BodyAtc] = new SphereCollider(scaling * 0.6f);
	EnableAttackColliders(false);

	using namespace MouseMobState;
	// 基本ステート
	stateMachine->RegisterState(MouseMob::State::Search, new MouseMobState::SearchState(this));
	stateMachine->RegisterState(MouseMob::State::Battle, new MouseMobState::BattleState(this));
	stateMachine->RegisterState(MouseMob::State::Recieve, new MouseMobState::RecievedState(this));

	// 移動サブステート
	stateMachine->RegisterSubState(MouseMob::State::Search, Search::Wander, new MouseMobState::WanderState(this, 1.0f));
	stateMachine->RegisterSubState(MouseMob::State::Search, Search::Idle, new MouseMobState::IdleState(this, 3.0f, 5.0f));

	// 攻撃ステート
	stateMachine->RegisterSubState(MouseMob::State::Battle, Battle::Pursuit, new MouseMobState::PursuitState(this, 2.0f, m_AttackRange, 3.0f, 5.0f));
	stateMachine->RegisterSubState(MouseMob::State::Battle, Battle::Attack, new MouseMobState::AttackState(this, 0.5f));
	stateMachine->RegisterSubState(MouseMob::State::Battle, Battle::Standby, new MouseMobState::StandbyState(this));

	// MetaAIのためステート
	stateMachine->RegisterSubState(MouseMob::State::Recieve, Recieve::Called, new MouseMobState::CalledState(this));

	stateMachine->SetState(MouseMob::State::Search);
}

// 一番近いプレイヤーをターゲット
void MouseMob::UpdateTarget()
{
	target = GetClosestPlayer(m_SearchRange);
}

// 衝突判定更新
void MouseMob::UpdateColliders()
{
	// 当たり判定
	colliders[HitCollider::BodyHit]->SetPosition(GetNodePosition("JOT_C_Body", DirectX::XMFLOAT3{ 0.0f, 50.0f, 0.0f } *scale));

	// 攻撃判定
	attackColliders[AttackCollider::BodyAtc]->SetPosition(GetNodePosition("JOT_C_Head", DirectX::XMFLOAT3{ 20.0f, 70.0f, 0.0f } *scale));
}