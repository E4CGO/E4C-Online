//! @file MouseMobState.cpp
//! @note 

#include "MouseMobState.h"

#include "TAKOEngine/Tool/Mathf.h"

namespace enemy
{

	namespace mouseMob
	{
		/**************************************************************************//**
			@brief	探しステートに入れる
		*//***************************************************************************/
		void SearchState::Enter()
		{
			SetSubState(SEARCH_STATE::IDLE);
		}
		/**************************************************************************//**
			@brief	探しスステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void SearchState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);
		}

		/**************************************************************************//**
			@brief　探しステートから出る
		*//***************************************************************************/
		void SearchState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	バトルステートに入れる
		*//***************************************************************************/
		void BattleState::Enter()
		{
			SetSubState(BATTLE_STATE::PURSUIT);
		}

		/**************************************************************************//**
			@brief	バトルステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void BattleState::Execute(float elapsedTime)
		{
			subState->Execute(elapsedTime);
		}

		/**************************************************************************//**
			@brief　バトルステートから出る
		*//***************************************************************************/
		void BattleState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	移動ステートに入れる
		*//***************************************************************************/
		void WanderState::Enter()
		{
			owner->SetRandomMoveTargetPosition();
		}

		/**************************************************************************//**
			@brief	移動ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void WanderState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();

			// 目的地点までのXZ平面での距離判定
			float distSq = XMFLOAT3HorizontalLengthSq(owner->GetPosition() - owner->GetMoveTargetPosition());

			// 目的地へ着いた
			if (distSq < m_ArrivedRadius * m_ArrivedRadius)
			{
				// 待機ステートへ遷移
				owner->GetStateMachine()->ChangeSubState(SEARCH_STATE::IDLE);
			}

			// 目的地点へ移動
			owner->MoveTo(elapsedTime, DirectX::XMFLOAT3(owner->GetMoveTargetPosition().x, owner->GetPosition().y, owner->GetMoveTargetPosition().z));

			// プレイヤー索敵
			if (owner->SearchPlayer())
			{
				owner->GetStateMachine()->ChangeState(STATE::BATTLE);
			}
		}

		/**************************************************************************//**
			@brief　移動ステートから出る
		*//***************************************************************************/
		void WanderState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	待機ステートに入れる
		*//***************************************************************************/
		void IdleState::Enter()
		{
			m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);
		}

		/**************************************************************************//**
			@brief	待機ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void IdleState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();

			// タイマー処理
			m_StateTimer -= elapsedTime;

			// 待機時間が経過したとき徘徊ステートへ遷移
			if (m_StateTimer <= 0.f)
			{
				owner->GetStateMachine()->ChangeSubState(SEARCH_STATE::WANDER);
			}

			// プレイヤーが見つかったときバトルステートへ遷移
			if (owner->SearchPlayer())
			{
				owner->GetStateMachine()->ChangeState(mouseMob::STATE::BATTLE);
			}
		}

		/**************************************************************************//**
			@brief　待機ステートから出る
		*//***************************************************************************/
		void IdleState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	追いかけるステートに入れる
		*//***************************************************************************/
		void PursuitState::Enter()
		{
			m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);
			m_AttackTimer = m_AttackCooldown;
		}

		/**************************************************************************//**
			@brief	追いかけるステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void PursuitState::Execute(float elapsedTime)
		{
			// タイマー処理
			m_StateTimer -= elapsedTime;
			m_AttackTimer -= elapsedTime;

			float distSq = XMFLOAT3LengthSq(owner->GetPosition() - owner->GetTarget()->GetPosition());

			// 目的地点へ移動
			if (distSq > m_AttackRange * m_AttackRange)
			{
				owner->MoveTo(elapsedTime, owner->GetTarget()->GetPosition());
			}

			owner->TurnTo(elapsedTime, owner->GetTarget()->GetPosition());

			// 攻撃範囲に入ったとき攻撃ステートへ遷移
			if (distSq <= m_AttackRange * m_AttackRange)
			{
				if (m_AttackTimer <= 0.f)
				{
					owner->GetStateMachine()->ChangeSubState(BATTLE_STATE::ATTACK);
				}
			}

			// 追跡時間が経過したとき探しステートへ遷移
			if (m_StateTimer <= 0.f)
			{
				owner->GetStateMachine()->ChangeState(mouseMob::STATE::SEARCH);
			}
		}

		/**************************************************************************//**
			@brief　追いかけるステートから出る
		*//***************************************************************************/
		void PursuitState::Exit()
		{
			m_AttackTimer = m_AttackCooldown;
		}

		/**************************************************************************//**
			@brief	攻撃ステートに入れる
		*//***************************************************************************/
		// 攻撃ステート
		void AttackState::Enter()
		{
			m_AnimationTimer = m_WaitTimer;
		}

		/**************************************************************************//**
			@brief	攻撃ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void AttackState::Execute(float elapsedTime)
		{
			m_AnimationTimer -= elapsedTime;
			if (m_AnimationTimer <= 0)
			{
				owner->GetStateMachine()->ChangeSubState(BATTLE_STATE::PURSUIT);
			}
		}

		void AttackState::Exit()
		{
		}
	}
}