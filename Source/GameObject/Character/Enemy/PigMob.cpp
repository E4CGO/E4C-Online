//! @file PigMob.cpp
//! @note 

#include "PigMob.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "GameObject/Character/Enemy/PigMobState.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "Map/MapTileManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    scaling	スケール
	@param[in]    renderMode	レンダリング設定
*//***************************************************************************/
PigMob::PigMob(float scaling, ModelObject::RENDER_MODE renderMode) : Enemy("Data/Model/Enemy/MDLANM_ENMpig_0120.glb", scaling, renderMode)
{
	// 敵の基本パラメーター
	enemyType = ENEMY_TYPE::PIG;
	radius = 1.0f;
	maxHp = hp = 80;
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
		stateMachine->RegisterState(Enemy::STATE::IDLE, new IdleState(this, 1.0f));
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
	@brief	壁衝突時
*//***************************************************************************/
void PigMob::OnWall()
{
	stateMachine->ChangeState(STATE::SHOT);
}

/**************************************************************************//**
 	@brief	発射物生成
*//***************************************************************************/
void PigMob::Shot()
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById(GetTarget());
	if (player == nullptr) return;

	// コインを飛ばす
	Projectile* coin = PROJECTILES.Register(new CoinBullet(player->GetPosition(), this));

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

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]    target	
	@param[in]    owner	
*//***************************************************************************/
CoinBullet::CoinBullet(DirectX::XMFLOAT3 target, Character* owner) : Projectile("Data/Model/Object/coin.glb", 2.0f, owner)
{
	m_radius = 0.5f;
	m_power = 10;
	m_speed = 10.0f;
	m_turnSpeed = DirectX::XM_2PI;
	m_existTime = 5.0f;
	m_currentTimer = m_existTime;
	m_pmodels[0]->GetRootNode()->position = {};	// モデルがちゃんとしてたらこの文はいらない

	SetMoveCollider({ {0, 0, 0}, m_radius }, Collider::COLLIDER_OBJ::ENEMY_PROJECTILE);
	m_pColliders.clear();
	MakeAttackCollider(m_power, 0, { {}, m_radius }, Collider::COLLIDER_OBJ::ENEMY_PROJECTILE, Collider::COLLIDER_OBJ::PLAYER, &transform);
	m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });

	DirectX::XMFLOAT4X4 matrix = owner->GetModel()->FindNode("Pig_tail_jnt5")->worldTransform;
	SetPosition({ matrix._41, matrix._42, matrix._43 });
	SetAngle(owner->GetAngle());
	//angle.y += DirectX::XM_PIDIV2;
	m_direction = XMFLOAT3Normalize({ target.x - position.x, 0, target.z - position.z });
}

void CoinBullet::Update(float elapsedTime)
{
	// 衝突判定はオーナーのプレイヤーが計算
	if (static_cast<PigMob*>(m_owner)->IsMine())
	{
		Sphere sphere;
		sphere.position = position;
		sphere.radius = m_radius;
		if (MAPTILES.IntersectSphereVsMap(sphere))
		{
			Destory();
		}
	}
	
	position += m_direction * m_speed * elapsedTime;
	
	float modelAngle = m_turnSpeed * elapsedTime;
	DirectX::XMFLOAT4* rot = &m_pmodels[0]->GetRootNode()->rotation;
	DirectX::XMMATRIX Q = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(rot));
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationAxis({ 1, 0, 0 }, modelAngle));
	Q = Q * R;
	DirectX::XMStoreFloat4(rot, DirectX::XMQuaternionRotationMatrix(Q));

	Projectile::Update(elapsedTime);

	if (m_currentTimer < 0.0f)
	{
		Destory();
	}
}

/**************************************************************************//**
	@brief	当たり判定処理
*//***************************************************************************/
void CoinBullet::CollisionFunction(Collider* myCol, Collider* otherCol)
{
	if (m_power > otherCol->GetArmor())
	{
		Character* chara = static_cast<Character*>(otherCol->GetOwner());
		if (chara->IsGround())
		{
			uint16_t damage = m_power - otherCol->GetArmor();
			chara->OnDamage(damage);
		}
	}

	myCol->SetEnable(false);
	Destory();
}
