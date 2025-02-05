#include "ParticleObject.h"
#include "Map/MapTileManager.h"
#include "TAKOEngine/Physics/AttackCollider.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
ParticleObject::ParticleObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/arrow.glb", 1.0f, owner)
{
	m_power = 1;
	m_coolTime = 0.5f;
	m_currentTimer = m_coolTime;
	m_direction = XMFLOAT3Normalize(owner->GetTarget());
	SetMoveCollider({ {0, 0, 0}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	m_pColliders.clear();

	DirectX::XMFLOAT3 startPosition = { owner->GetPosition().x, owner->GetPosition().y + owner->GetHeight() * .6f, owner->GetPosition().z };
	SetPosition(startPosition + owner->GetFront() * m_distance);
	if (owner->IsPlayer())
	{
		MakeAttackCollider(m_power, 0, { {}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, &transform);
		m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
	}
	
	//SetPosition(startPosition);
	
	//if (owner->IsPlayer())
	//{
	//	Capsule sphereCast;
	//	sphereCast.position = position;
	//	sphereCast.direction = m_direction;
	//	sphereCast.length = m_distance;
	//	sphereCast.radius = m_radius;

	//	if (MAPTILES.IntersectCapsuleVsMap(sphereCast))
	//	{
	//		SetPosition(sphereCast.position + sphereCast.direction * sphereCast.length);
	//	}
	//	else
	//	{
	//		SetPosition(sphereCast.position + sphereCast.direction * sphereCast.length);
	//	}

	//}
	//else
	//{
	//	SetPosition(position + owner->GetFront() * m_distance);
	//}
	
	m_particle = std::make_unique<HitParticleRenderer>(position);
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void ParticleObject::Update(float elapsedTime)
{
	PlayerCharacter* player = static_cast<PlayerCharacter*>(m_pOwner);

	// 時間経過でAttackColliderを再生成
	if (m_currentTimer < 0.0f)
	{
		if (player->IsPlayer())
		{
			DeleteAttackCollider(0);
			MakeAttackCollider(m_power, 0, { {}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, &transform);
			m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
		}
		m_currentTimer = m_coolTime;
	}

	// 別のステートに変化すると消える
	if (player->GetState() != PlayerCharacter::STATE::ATTACK_SPECIAL)
	{
		Destory();
	}

	// MPが尽きたら消える
	if (player->GetMp() < 10.0f * elapsedTime)
	{
		Destory();
	}

	Projectile::Update(elapsedTime);
}

/**************************************************************************//**
	@brief	当たり判定処理
*//***************************************************************************/
void ParticleObject::CollisionFunction(Collider* myCol, Collider* otherCol)
{
	AttackSphereCollider* attack = static_cast<AttackSphereCollider*>(myCol);
	for (GameObject* owner : attack->GetHitOthers())
	{
		// 既にヒットした敵には当たらない
		if (owner == otherCol->GetOwner()) return;
	}

	if (m_power > otherCol->GetArmor())
	{
		ModelObject* owner = static_cast<ModelObject*>(otherCol->GetOwner());
		uint16_t damage = m_power - otherCol->GetArmor();
		attack->RegisterHitOthers(owner);
		owner->OnDamage(damage);
	}
}

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void ParticleObject::RenderDX12(const RenderContextDX12& rc)
{
	//ModelObject::RenderDX12(rc);
	m_particle->Render(rc);
}