#include "BeamObject.h"

#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "GameObject/Character/Enemy/EnemyManager.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Physics/AttackCollider.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
BeamObject::BeamObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/EffectBeam.glb", 1.0f, owner)
{
	SetShader("EffectBeam", ModelShaderDX12Id::Beam);

	//DirectX::XMFLOAT3 startPosition = { owner->GetPosition().x, owner->GetPosition().y + 1.0f, owner->GetPosition().z };
	//DirectX::XMVECTOR vStartPosition = DirectX::XMLoadFloat3(&startPosition);
	//DirectX::XMFLOAT3 front = owner->GetFront();
	//DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	//direction = DirectX::XMVectorScale(direction, m_distance);
	//DirectX::XMVECTOR newPosition = XMVectorAdd(vStartPosition, direction);
	//DirectX::XMStoreFloat3(&position, newPosition);

	//SetAngle(owner->GetAngle());
	//RotateAxis({ 0.0f, 1.0f, 0.0f }, XMConvertToRadians(180.0f));

	scale.z = 0.0f;

	m_power = 30;
	m_existTime = 1.5f;
	m_coolTime = 0.5f;
	m_currentTimer = m_coolTime;
	//m_direction = XMFLOAT3Normalize(owner->GetTarget() - position);

	if (owner->IsPlayer())
	{
		m_pColliders.clear();
		MakeAttackCollider(m_power, 0, { {}, {0, 0, -1}, m_length * scale.z, m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, &transform);
		m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
	}
	//SetPosition(startPosition);

	//m_beam = std::make_unique<ModelObject>("Data/Model/Object/EffectBeam.glb", 1.0f, ModelObject::DX12, ModelObject::LHS_TOON);
	//m_beam->SetShader("EffectBeam", ModelShaderDX12Id::HealCylinder);

	//m_beam->SetPosition(startPosition);

	//m_beam->SetAngle(owner->GetAngle());
	//m_beam->RotateAxis({ 0.0f, 1.0f, 0.0f }, XMConvertToRadians(180.0f));
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void BeamObject::Update(float elapsedTime)
{
	PlayerCharacter* player = static_cast<PlayerCharacter*>(m_pOwner);
	
	// 別のステートに変化すると消える
	if (player->GetState() != PlayerCharacter::STATE::SKILL_1)
	{
		Destory();
		return;
	}

	// 発射時間まで何もしない
	if (m_pOwner->GetModel()->GetCurrentAnimationSeconds() < 0.65f) return;

	//SetAngle(player->GetAngle());
	//RotateAxis({ 0.0f, 1.0f, 0.0f }, XMConvertToRadians(180.0f));
	//RotateAxis({ 1, 0, 0 }, AngleBetweenXMFLOAT3({ 0, 0, 1 }, { 0, m_direction.y, 1 }));

	if (m_isExtend)
	{
		//scale.z = 0.5f * (1 + (m_coolTime - m_currentTimer) / m_coolTime);
		scale.z = (m_coolTime - m_currentTimer) / m_coolTime;
		if (m_currentTimer < 0.0f)
		{
			scale.z = 1.0f;
			m_isExtend = false;
		}

		if (player->IsPlayer())
			m_pColliders[0]->SetParam({ {}, {0, 0, -1}, m_length * scale.z, m_radius });
	}

	// 時間経過でAttackColliderを再生成
	if (m_currentTimer < 0.0f && m_existTime > 0.0f)
	{
		if (player->IsPlayer())
		{
			DeleteAttackCollider(0);
			MakeAttackCollider(m_power, 0, { {}, {0, 0, -1}, m_length * scale.z , m_radius }, Collider::COLLIDER_OBJ::PLAYER_PROJECTILE, Collider::COLLIDER_OBJ::ENEMY, & transform);
			m_pColliders[0]->SetCollisionFunction([&](Collider* myCol, Collider* otherCol) {CollisionFunction(myCol, otherCol); });
		}
		m_currentTimer = m_coolTime;
	}

	// 細くなって消える
	if (m_existTime < 0.0f)
	{
		if(!m_pColliders.empty()) 	DeleteAttackCollider(0);

		scale.x = scale.x > 0.0f ? 1.0f + m_existTime : 0.0f;
		scale.y = scale.y > 0.0f ? 1.0f + m_existTime : 0.0f;
	}

	//Projectile::Update(elapsedTime); // 位置更新
	{	// スケール行列生成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		// 回転行列生成
		DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
		DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
		DirectX::XMMATRIX R = X * Y;
		// 位置行列生成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX W = S * R * T;
		DirectX::XMStoreFloat4x4(&transform, W);

		if (T_GRAPHICS.isDX11Active)
		{
			for (auto& model : m_pmodels)
			{
				if (model == nullptr) continue;

				// アニメーション更新
				model->UpdateAnimation(elapsedTime * m_animationSpeed);

				// トランスフォーム更新
				model->UpdateTransform(transform);
			}
		}

		if (T_GRAPHICS.isDX12Active)
		{
			for (auto& model : m_pmodels)
			{
				if (model == nullptr) continue;

				// アニメーション更新
				model->UpdateAnimation(elapsedTime * m_animationSpeed);

				// トランスフォーム更新
				model->UpdateTransform(transform);
			}
		}

		UpdateColliders();
		m_currentTimer -= elapsedTime;
		m_existTime -= elapsedTime; 
	}
}

///**************************************************************************//**
//	@brief	当たり判定処理
//*//***************************************************************************/
void BeamObject::CollisionFunction(Collider* myCol, Collider* otherCol)
{
	AttackCapsuleCollider* attack = static_cast<AttackCapsuleCollider*>(myCol);
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
void BeamObject::RenderDX12(const RenderContextDX12& rc)
{
	//m_beam->RenderDX12(rc);

	if (m_pOwner->GetModel()->GetCurrentAnimationSeconds() > 0.65f)
	{
		ModelObject::RenderDX12(rc);
	}
}