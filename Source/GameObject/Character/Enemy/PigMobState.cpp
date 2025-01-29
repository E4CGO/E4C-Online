//! @file PigMobState.cpp
//! @note 

#include "PigMobState.h"

namespace EnemyState
{
	namespace PigMob
	{
		/**************************************************************************//**
			@brief	待機ステートに入る
		*//***************************************************************************/
		void IdleState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_IDLE, true, 0.1f);
			::EnemyState::IdleState::Enter();
		}

		/**************************************************************************//**
			@brief	待機ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void IdleState::Execute(float elapsedTime)
		{
			if (!owner->IsMine()) return;
			EnemyState::IdleState::Execute(elapsedTime);

			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				if (!owner->IsReloaded())
				{
					// コイン刺青
					EnemyState::StateTransition(owner, ::PigMob::STATE::RELOARD);
				}
				else if (!IsWaiting())
				{
					// ターゲットに距離を取る
					EnemyState::StateTransition(owner, ::PigMob::STATE::AWAY);
				}
			}
			else if (!IsWaiting())
			{
				// 徘徊
				owner->SetTarget(nullptr);
				owner->SetTargetPosition(GetRandomPointInCircleArea(owner->GetPosition(), 5.0f));
				EnemyState::StateTransition(owner, ::PigMob::STATE::WANDER);
			}
		}

		/**************************************************************************//**
			@brief	徘徊ステートに入る
		*//***************************************************************************/
		void MoveState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_MOVE, true);
		}
		/**************************************************************************//**
			@brief		徘徊ステートで実行する
			@param[in]    elapsedTime	経過時間
		*//***************************************************************************/
		void MoveState::Execute(float elapsedTime)
		{
			owner->UpdateTarget();
			if (owner->GetTarget() < UINT32_MAX)
			{
				// ターゲット発見
				EnemyState::StateTransition(owner, ::PigMob::STATE::RELOARD);
				return;
			}
			::EnemyState::MoveState::Execute(elapsedTime);
		}
		
		/**************************************************************************//**
		 	@brief	リロードステートに入る
		*//***************************************************************************/
		void ReloadState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_RELOAD, false, 0.1f);
			owner->SetReload(true);
		}
		void ReloadState::Execute(float elapsedTime)
		{
			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
			}
		}

		/**************************************************************************//**
			@brief 離れるステートに入る
		*//***************************************************************************/
		void AwayState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_MOVE, true);
		}
		/**************************************************************************//**
			@brief		離れるステート
			@param[in]	elapsedTime 経過時間
		*//***************************************************************************/
		void AwayState::Execute(float elapsedTime)
		{
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player == nullptr)
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
				return;
			}
			DirectX::XMFLOAT3 diff = player->GetPosition() - owner->GetPosition();
			if (owner->IsWall() || XMFLOAT3HorizontalLengthSq(diff) > 144.0f)
			{
				// 射撃開始
				EnemyState::StateTransition(owner, ::PigMob::STATE::SHOT);
			}
			else
			{
				// ターゲットから距離を取る
				owner->MoveTo(elapsedTime, player->GetPosition() - (diff * 6.0f));
			}
		}
		/**************************************************************************//**
		 	@brief	射撃ステートに入る
		*//***************************************************************************/
		void ShotState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_SHOT, false);
			m_isShot = false;

			m_tempTurnSpeed = owner->GetTurnSpeed();
			owner->SetTurnSpeed(m_tempTurnSpeed * 3.0f);
		}
		/**************************************************************************//**
		 	@brief		射撃ステートを実行する
			@param[in]	elapsedTime
		*//***************************************************************************/
		void ShotState::Execute(float elapsedTime)
		{

			if (!owner->IsPlayAnimation())
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
				return;
			}
			PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(owner->GetTarget());
			if (player == nullptr)
			{
				EnemyState::StateTransition(owner, Enemy::STATE::IDLE);
				return;
			}

			if (owner->GetModel()->GetAnimationRate() > 0.56f && !m_isShot)
			{
				owner->Shot();
				m_isShot = true;
			}
			else
			{
				owner->TurnTo(elapsedTime, player->GetPosition());
			}
		}
		/**************************************************************************//**
			@brief	射撃ステートから出る
		*//***************************************************************************/
		void ShotState::Exit()
		{
			owner->SetTurnSpeed(m_tempTurnSpeed);
		}

		/**************************************************************************//**
			@brief	ダメージステート
		*//***************************************************************************/
		void HurtState::Enter()
		{
			owner->SetAnimation(::PigMob::ANIM_DAMAGE, false, 0.1f);
		}

		/**************************************************************************//**
			@brief	死亡ステート
		*//***************************************************************************/
		void DeathState::Enter()
		{
			EnemyState::DeathState::Enter();
			owner->SetAnimation(::PigMob::ANIMATION::ANIM_DIE, false, 0.1f);
		}
	};
};
