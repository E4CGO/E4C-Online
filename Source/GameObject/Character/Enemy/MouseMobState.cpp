//! @file MouseMobState.cpp
//! @note 

#include "MouseMobState.h"

#include "TAKOEngine/Tool/Mathf.h"

namespace EnemyState
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
			SetSubState(BATTLE_STATE::ENCOUNTER);
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
		
			owner->SetAnimation(MouseMob::ANIMATION::ANIM_MOVE, true, 0.1f);
		}

		/**************************************************************************//**
			@brief	移動ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void WanderState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();

			// 目的地点までのXZ平面での距離判定
			float distSq = XMFLOAT3HorizontalLengthSq(owner->GetPosition() - owner->GetTargetPosition());

			// 目的地へ着いた
			if (distSq < m_ArrivedRadius * m_ArrivedRadius)
			{
				// 待機ステートへ遷移
				owner->GetStateMachine()->ChangeSubState(SEARCH_STATE::IDLE);
			}

			// 目的地点へ移動
			owner->MoveTo(elapsedTime, DirectX::XMFLOAT3(owner->GetTargetPosition().x, owner->GetPosition().y, owner->GetTargetPosition().z));

			// プレイヤー索敵
			if (owner->SearchPlayer())
			{
				EnemyState::StateTransition(owner, STATE::BATTLE);
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
			//m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);

			owner->SetAnimation(MouseMob::ANIMATION::ANIM_IDLE, true, 0.1f);
		}

		/**************************************************************************//**
			@brief	待機ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void IdleState::Execute(float elapsedTime)
		{
			//owner->UpdateTarget();

			//// タイマー処理
			//m_StateTimer -= elapsedTime;

			//// 待機時間が経過したとき徘徊ステートへ遷移
			//if (m_StateTimer <= 0.f)
			//{
			//	owner->GetStateMachine()->ChangeSubState(SEARCH_STATE::WANDER);
			//}

			//// プレイヤーが見つかったときバトルステートへ遷移
			//if (owner->SearchPlayer())
			//{
			//	owner->GetStateMachine()->ChangeState(mouseMob::STATE::BATTLE);
			//}

			if (!owner->IsMine()) return;
			EnemyState::IdleState::Execute(elapsedTime);

			if (!IsWaiting()) // 待機時間終わり
			{
				for (PlayerCharacter* player : PlayerCharacterManager::Instance().GetAll())
				{
					if (XMFLOAT3HorizontalLengthSq(player->GetPosition() - owner->GetPosition()) < 10.0f * 10.0f)
					{
						owner->AddHate(player->GetClientId(), 1);
					}
				}


				PlayerCharacter* player = owner->GetHighestHateClient();
				if (player != nullptr)
				{
					owner->SetTarget(player);
					EnemyState::StateTransition(owner, STATE::BATTLE);
				}
				else
				{

					owner->SetTarget(nullptr);
					owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 10.0f));
					owner->GetStateMachine()->ChangeSubState(SEARCH_STATE::WANDER);
				}
			}
		}

		/**************************************************************************//**
			@brief　待機ステートから出る
		*//***************************************************************************/
		void IdleState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	エンカウントステートに入れる
		*//***************************************************************************/
		void EncounterState::Enter()
		{
			owner->SetAnimation(MouseMob::ANIMATION::ANIM_ENCOUNTER, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	エンカウントステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void EncounterState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				owner->GetStateMachine()->ChangeSubState(BATTLE_STATE::PURSUIT);
			}
		}

		/**************************************************************************//**
			@brief	エンカウントステートから出る
		*//***************************************************************************/
		void EncounterState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	追いかけるステートに入れる
		*//***************************************************************************/
		void PursuitState::Enter()
		{
			m_StateTimer = Mathf::RandomRange(m_MinWaitingTime, m_MaxWaitingTime);
			m_AttackTimer = m_AttackCooldown;

			owner->SetAnimation(MouseMob::ANIMATION::ANIM_MOVE, true, 0.1f);
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

			PlayerCharacter* target = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (target == nullptr)
			{
				::EnemyState::StateTransition(owner, ::Enemy::STATE::IDLE);
			}

			float distSq = XMFLOAT3LengthSq(owner->GetPosition() - target->GetPosition());

			// 目的地点へ移動
			if (distSq > m_AttackRange * m_AttackRange)
			{
				owner->MoveTo(elapsedTime, target->GetPosition());
			}

			owner->TurnTo(elapsedTime, target->GetPosition());

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
				EnemyState::StateTransition(owner, STATE::SEARCH);
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
		void AttackState::Enter()
		{
			m_AnimationTimer = m_WaitTimer;

			owner->SetAnimation(MouseMob::ANIMATION::ANIM_ATTACK, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	攻撃ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void AttackState::Execute(float elapsedTime)
		{
			m_AnimationTimer -= elapsedTime;
			//if (m_AnimationTimer <= 0)
			if (!owner->IsPlayAnimation())
			{
				owner->GetStateMachine()->ChangeSubState(BATTLE_STATE::PURSUIT);
			}
		}
		void AttackState::Exit()
		{
		}

		/**************************************************************************//**
			@brief	ダメージステート
		*//***************************************************************************/
		void HurtState::Enter()
		{
			owner->SetAnimation(MouseMob::ANIMATION::ANIM_DAMAGE, false, 0.1f);
		}
		void HurtState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				StateTransition(owner, mouseMob::SEARCH);
			}
		}
		void HurtState::Exit()
		{
		}
		
		/**************************************************************************//**
			@brief	死亡ステート
		*//***************************************************************************/
		void DeathState::Enter()
		{
			EnemyState::DeathState::Enter();
			owner->SetAnimation(MouseMob::ANIMATION::ANIM_DAMAGE, false, 0.1f);
		}
		void DeathState::Execute(float elapsedTime)
		{
			EnemyState::DeathState::Execute(elapsedTime);
		}
		void DeathState::Exit()
		{
		}
	}
}