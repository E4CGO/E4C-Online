//! @file MouseMobState.cpp
//! @note 

#include "MouseMobState.h"

#include "TAKOEngine/Tool/Mathf.h"

/**************************************************************************//**
	@brief	探しステートに入れる
*//***************************************************************************/
void MouseMobState::SearchState::Enter()
{
	SetSubState(Search::Idle);
}
/**************************************************************************//**
	@brief	探しスステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::SearchState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

/**************************************************************************//**
	@brief　探しステートから出る
*//***************************************************************************/
void MouseMobState::SearchState::Exit()
{
}

/**************************************************************************//**
	@brief	バトルステートに入れる
*//***************************************************************************/
void MouseMobState::BattleState::Enter()
{
	SetSubState(Battle::Pursuit);
}

/**************************************************************************//**
	@brief	バトルステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::BattleState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

/**************************************************************************//**
	@brief　バトルステートから出る
*//***************************************************************************/
void MouseMobState::BattleState::Exit()
{
}

/**************************************************************************//**
	@brief	MetaAIステートに入れる
*//***************************************************************************/
void MouseMobState::RecievedState::Enter()
{
	SetSubState(Recieve::Called);
}

/**************************************************************************//**
	@brief	MetaAIステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::RecievedState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

/**************************************************************************//**
	@brief　MetaAIテートから出る
*//***************************************************************************/
void MouseMobState::RecievedState::Exit()
{
}

/**************************************************************************//**
	@brief	移動ステートに入れる
*//***************************************************************************/
void MouseMobState::WanderState::Enter()
{
	owner->SetRandomMoveTargetPosition();
}

/**************************************************************************//**
	@brief	移動ステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::WanderState::Execute(float elapsedTime)
{
	owner->UpdateTarget();

	// 目的地点までのXZ平面での距離判定
	float dist = Mathf::CalcDistFloat3XZ(owner->GetPosition(), owner->GetMoveTargetPosition());

	// 目的地へ着いた
	if (dist * dist < m_ArrivedRadius * m_ArrivedRadius)
	{
		// 待機ステートへ遷移
		owner->GetStateMachine()->ChangeSubState(Search::Idle);
	}

	// 目的地点へ移動
	owner->MoveTo(elapsedTime, DirectX::XMFLOAT3(owner->GetMoveTargetPosition().x, owner->GetPosition().y, owner->GetMoveTargetPosition().z));

	// プレイヤー索敵
	if (owner->SearchPlayer())
	{
		owner->GetStateMachine()->ChangeState(MouseMob::State::Battle);
	}
}

/**************************************************************************//**
	@brief　移動ステートから出る
*//***************************************************************************/
void MouseMobState::WanderState::Exit()
{
}

/**************************************************************************//**
	@brief	待機ステートに入れる
*//***************************************************************************/
void MouseMobState::IdleState::Enter()
{
	m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);
}

/**************************************************************************//**
	@brief	待機ステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::IdleState::Execute(float elapsedTime)
{
	owner->UpdateTarget();

	// タイマー処理
	m_StateTimer -= elapsedTime;

	// 待機時間が経過したとき徘徊ステートへ遷移
	if (m_StateTimer <= 0.f)
	{
		owner->GetStateMachine()->ChangeSubState(Search::Wander);
	}

	// プレイヤーが見つかったときバトルステートへ遷移
	if (owner->SearchPlayer())
	{
		owner->GetStateMachine()->ChangeState(MouseMob::State::Battle);
	}
}

/**************************************************************************//**
	@brief　待機ステートから出る
*//***************************************************************************/
void MouseMobState::IdleState::Exit()
{
}

/**************************************************************************//**
	@brief	追いかけるステートに入れる
*//***************************************************************************/
void MouseMobState::PursuitState::Enter()
{
	m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);
	m_AttackTimer = m_AttackCooldown;
}

/**************************************************************************//**
	@brief	追いかけるステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::PursuitState::Execute(float elapsedTime)
{
	// タイマー処理
	m_StateTimer -= elapsedTime;
	m_AttackTimer -= elapsedTime;

	float dist = Mathf::CalcDistFloat3(owner->GetPosition(), owner->GetTarget()->GetPosition());

	// 目的地点へ移動
	if (dist > m_AttackRange)
	{
		owner->MoveTo(elapsedTime, owner->GetTarget()->GetPosition());
	}

	owner->TurnTo(elapsedTime, owner->GetTarget()->GetPosition());

	// 攻撃範囲に入ったとき攻撃ステートへ遷移
	if (dist <= m_AttackRange)
	{
		if (m_AttackTimer <= 0.f)
		{
			owner->GetStateMachine()->ChangeSubState(Battle::Attack);
		}
	}

	// 追跡時間が経過したとき探しステートへ遷移
	if (m_StateTimer <= 0.f)
	{
		owner->GetStateMachine()->ChangeState(MouseMob::State::Search);
	}
}

/**************************************************************************//**
	@brief　追いかけるステートから出る
*//***************************************************************************/
void MouseMobState::PursuitState::Exit()
{
	m_AttackTimer = m_AttackCooldown;
}

/**************************************************************************//**
	@brief	攻撃ステートに入れる
*//***************************************************************************/
// 攻撃ステート
void MouseMobState::AttackState::Enter()
{
	m_AnimationTimer = m_WaitTimer;
}

/**************************************************************************//**
	@brief	攻撃ステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::AttackState::Execute(float elapsedTime)
{
	owner->GetAttackColliders()[MouseMob::AttackCollider::BodyAtc]->SetEnable(true);

	m_AnimationTimer -= elapsedTime;
	if (m_AnimationTimer <= 0)
	{
		owner->GetStateMachine()->ChangeSubState(Battle::Pursuit);
	}
}

/**************************************************************************//**
	@brief　攻撃ステートから出る
*//***************************************************************************/
void MouseMobState::AttackState::Exit()
{
	owner->EnableAttackColliders(false);
}

/**************************************************************************//**
	@brief	攻撃保つステートに入れる
*//***************************************************************************/
void MouseMobState::StandbyState::Enter()
{
}

/**************************************************************************//**
	@brief	攻撃保つステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::StandbyState::Execute(float elapsedTime)
{
}

/**************************************************************************//**
	@brief　攻撃保つステートから出る
*//***************************************************************************/
void MouseMobState::StandbyState::Exit()
{
}

/**************************************************************************//**
	@brief	メッセージを受け取りステートに入れる
*//***************************************************************************/
void MouseMobState::CalledState::Enter()
{
}

/**************************************************************************//**
	@brief	メッセージを受け取ステートで実行する
	@param[in]    elapsedTime	経過時間
*//***************************************************************************/
void MouseMobState::CalledState::Execute(float elapsedTime)
{
}

/**************************************************************************//**
	@brief　メッセージを受け取ステートから出る
*//***************************************************************************/
void MouseMobState::CalledState::Exit()
{
}