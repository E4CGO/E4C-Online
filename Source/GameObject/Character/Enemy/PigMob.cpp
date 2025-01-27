//! @file PigMob.cpp
//! @note 

#include "PigMob.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/Character/Enemy/PigMobState.h"
#include "TAKOEngine/Physics/CollisionManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    scaling	スケール
	@param[in]    renderMode	レンダリング設定
*//***************************************************************************/
PigMob::PigMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMpig_0120.glb", scaling, renderMode)
{
	// 敵の基本パラメーター
	enemyType = ENEMY_TYPE::PIG;
	radius = 0.6f;
	maxHp = hp = 200;
	atk = 10;
	moveSpeed = 4.0f;
	m_tempMoveSpeed = moveSpeed;
	turnSpeed = DirectX::XMConvertToRadians(360);
	SetSearchRange(24.0f);
	//m_AttackRange = 1.25f;
	m_AttackRange = 1.8f;
	SetReload(false);

	// 衝突判定
	SetMoveCollider({ { 0, radius / scale.y, 0 }, radius }, Collider::COLLIDER_OBJ::ENEMY);

	m_pColliders.clear();
	// ヒット判定
	Sphere sphere{ { 0, radius / scale.y, 0 }, radius };
	SetCollider(COLLIDER_ID::COL_BODY, sphere, Collider::COLLIDER_OBJ::ENEMY, &m_pmodels[0]->FindNode("Pig_mid_jnt")->worldTransform);
	{
		using namespace EnemyState::PigMob;
		stateMachine->RegisterState(Enemy::STATE::IDLE, new IdleState(this, 2.0f));
		stateMachine->SetState(Enemy::STATE::IDLE);

		stateMachine->RegisterState(::Enemy::STATE::HURT, new HurtState(this));
		stateMachine->RegisterState(::Enemy::STATE::DEATH, new DeathState(this));

		stateMachine->RegisterState(STATE::WANDER, new MoveState(this));
		stateMachine->RegisterState(STATE::RELOARD, new ReloadState(this));
		stateMachine->RegisterState(STATE::AWAY, new AwayState(this));
		stateMachine->RegisterState(STATE::SHOT, new ShotState(this));
	}
}

/**************************************************************************//**
 	@brief	発射物生成
*//***************************************************************************/
void PigMob::Shot()
{
	// TODO コインを飛ばす
	// リロードフラグオフ
	SetReload(false);
}

/**************************************************************************//**
 	@brief		リロード設定
	@param[in]	reloaded	フラグ
*//***************************************************************************/
void PigMob::SetReload(bool reloaded)
{
	m_isReloaded = reloaded;
	if (!m_isReloaded)
	{
		moveSpeed = 1.0f;
	}
	else
	{
		moveSpeed = m_tempMoveSpeed;
	}

}
