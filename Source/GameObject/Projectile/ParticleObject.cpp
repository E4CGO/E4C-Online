#include "ParticleObject.h"
#include "Map/MapTileManager.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "TAKOEngine/Physics/AttackCollider.h"
#include "UI/Widget/WidgetCrosshair.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
ParticleObject::ParticleObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/arrow.glb", 1.0f, owner)
{
	m_power = 15;
	m_coolTime = 0.5f;
	m_currentTimer = 0.7f;
	m_direction = XMFLOAT3Normalize(owner->GetTarget());
	//SetMoveCollider({ {0, 0, 0}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE);
	m_pColliders.clear();

	DirectX::XMFLOAT3 startPosition = { owner->GetPosition().x, owner->GetPosition().y + owner->GetHeight() * .6f, owner->GetPosition().z };
	SetPosition(startPosition + owner->GetFront() * m_distance);

	float tmpLength = m_distance * m_distance;
	for (Collider*& collider : COLLISIONS.GetAll())
	{
		if (collider->GetOBJType() != Collider::COLLIDER_OBJ::ENEMY)	continue;

		XMFLOAT3 horizontalV = { collider->GetPosition().x - owner->GetPosition().x, 0, collider->GetPosition().z - owner->GetPosition().z };
		float length = XMFLOAT3LengthSq(horizontalV);
		if (length < tmpLength)
		{
			tmpLength = length;
			SetPosition(collider->GetPosition());
		}
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

	if (owner->IsPlayer())
	{
		CROSS_HAIR.Show();
	}
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void ParticleObject::Update(float elapsedTime)
{
	PlayerCharacter* player = static_cast<PlayerCharacter*>(m_pOwner);
	player->Turn(elapsedTime, position.x - player->GetPosition().x, position.z - player->GetPosition().z, player->GetTurnSpeed());

	// 時間経過でAttackColliderを再生成
	if (m_currentTimer < 0.0f)
	{
		if (player->IsPlayer())
		{
			DeleteAttackCollider(0);
			MakeAttackCollider(m_power, 0, { {}, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, &transform);
			m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
		}
		//m_currentTimer = m_coolTime;

		Destory();
	}

	// 別のステートに変化すると消える
	if (player->GetState() != PlayerCharacter::STATE::ATTACK_SPECIAL)
	{
		Destory();
	}

	// MPが尽きたら消える
	else if (player->GetMp() < 10.0f * elapsedTime)
	{
		Destory();
	}

	Projectile::Update(elapsedTime);

	// CROSS_HAIRの位置設定
	if (player->IsPlayer())
	{
		// 変換行列
		const DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetView());
		const DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetProjection());
		const DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
		// カメラ
		const DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
		const DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();

		float dot = XMFLOAT3Dot(front, position - eye);
		if (dot > 0.0f)
		{
			DirectX::XMFLOAT3 pos = T_GRAPHICS.GetScreenPosition(position, T_GRAPHICS.GetViewPort(), View, Projection, World);
			CROSS_HAIR.SetPosition({ pos.x, pos.y });
		}
	}
}

/**************************************************************************//**
	@brief	消滅コールバック
*//***************************************************************************/
void ParticleObject::OnDestory()
{
	//PlayerCharacter* player = static_cast<PlayerCharacter*>(m_pOwner);
	//player->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);

	CROSS_HAIR.SetPosition({ SCREEN_W * 0.5f - CROSS_HAIR.GetSize().x * 0.5f, SCREEN_H * 0.5f - CROSS_HAIR.GetSize().y * 0.5f });
	CROSS_HAIR.Hide();
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