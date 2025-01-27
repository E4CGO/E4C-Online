#include "FireballObject.h"

//#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Map/MapTileManager.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
FireballObject::FireballObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/arrow.glb", 1.0f, owner)
{
	m_radius = 0.5f;
	m_power = 7;
	m_speed = 40.0f;
	m_existTime = 2.0f;
	m_chargeTime = 4.0f;
	m_currentTimer = m_chargeTime;

	SetMoveCollider({ {0, 0, 0}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	m_pColliders.clear();

	m_fireball = std::make_unique<PlaneDX12>("Data/Sprites/fire.png", 1.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f);
	m_fireball->SetShaderDX12(ModelShaderDX12Id::Fireball);
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void FireballObject::Update(float elapsedTime)
{
	if(m_isCharge)
	{
		PlayerCharacter* player = static_cast<PlayerCharacter*>(m_owner);

		if (player->IsPlayer())
		{
			if (player->InputAttackNormal())
			{				
				float s = (m_chargeTime - m_currentTimer) * m_radius * 0.5f;
				scale = { s, s, s };
				float radius = m_radius * s;
				m_pMoveCollider->SetParam({ {}, radius });

				position = player->GetShotPosition();
				Projectile::Update(elapsedTime);

				DirectX::XMFLOAT3 offsetPos;
				offsetPos.x = position.x - 1.0f * scale.x;
				offsetPos.y = position.y + 1.0f * scale.y;
				offsetPos.z = position.z + 0.0f * scale.z;
				m_fireball->SetPosition(offsetPos);
				m_fireball->SetScale(scale);
				m_fireball->Update(elapsedTime);
			}
			else if (player->ReleaseAttackNormal())
			{
				m_isCharge = false;
				m_currentTimer = m_existTime;
				m_direction = XMFLOAT3Normalize(player->GetTarget() - position);
				MakeAttackCollider(m_power, 0, { {}, m_direction, m_speed, m_radius * scale.x }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, &transform);
				m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
			}
		}
		else
		{

		}

		// ダメージを受けたら消える
		if (player->GetState() == PlayerCharacter::STATE::HURT || player->GetState() == PlayerCharacter::STATE::DEATH)
		{
			Destory();
		}

		if (m_currentTimer < 0.0f) m_currentTimer = 0.0f;
	}
	else
	{
		DirectX::XMFLOAT3 velocity = m_direction * m_speed * elapsedTime;

		// 衝突判定はオーナーのプレイヤーが計算
		if (static_cast<PlayerCharacter*>(m_owner)->IsPlayer())
		{
			Capsule sphereCast;
			sphereCast.position = position;
			sphereCast.direction = m_direction;
			sphereCast.length = m_speed * elapsedTime;
			sphereCast.radius = m_pMoveCollider->GetSphere().radius;
			m_pColliders[0]->SetParam({ {}, m_direction, sphereCast.length, sphereCast.radius });

			if (MAPTILES.IntersectCapsuleVsMap(sphereCast))
			{
				Destory();
			}
		}
		position += velocity;

		Projectile::Update(elapsedTime);

		DirectX::XMFLOAT3 offsetPos;
		offsetPos.x = position.x - 1.0f * scale.x;
		offsetPos.y = position.y + 1.0f * scale.y;
		offsetPos.z = position.z + 0.0f * scale.z;
		m_fireball->SetPosition(offsetPos);
		m_fireball->Update(elapsedTime);
		
		if (m_currentTimer < 0.0f)
		{
			Destory();
		}
	}
}

/**************************************************************************//**
	@brief	当たり判定処理
*//***************************************************************************/
void FireballObject::CollisionFunction(Collider* myCol, Collider* otherCol)
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

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void FireballObject::RenderDX12(const RenderContextDX12& rc)
{
	//ModelObject::RenderDX12(rc);
	m_fireball->RenderDX12(rc);
}