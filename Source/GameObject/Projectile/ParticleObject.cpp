#include "ParticleObject.h"

/**************************************************************************//**
	@brief    コンテクスト
	@param[in]    owner	プレイヤー
*//***************************************************************************/
ParticleObject::ParticleObject(PlayerCharacter* owner) : Projectile("Data/Model/Object/arrow.glb", 1.0f, owner)
{
	DirectX::XMFLOAT3 startPosition = { owner->GetPosition().x, owner->GetPosition().y + owner->GetHeight() * .5f, owner->GetPosition().z };
	DirectX::XMVECTOR vStartPosition = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMFLOAT3 front = owner->GetFront();
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	direction = DirectX::XMVectorScale(direction, m_distance);
	DirectX::XMVECTOR newPosition = XMVectorAdd(vStartPosition, direction);
	DirectX::XMStoreFloat3(&startPosition, newPosition);

	m_particle = std::make_unique<HitParticleRenderer>(startPosition);
}

/**************************************************************************//**
	@brief		変更
	@param[in]    elapsedTime
*//***************************************************************************/
void ParticleObject::Update(float elapsedTime)
{
}

/**************************************************************************//**
	@brief	当たり判定処理
*//***************************************************************************/
//void ParticleObject::Collision()
//{
//}

/**************************************************************************//**
	@brief		ダメージ計算
	@param[in]    hit	相手の当たり判定
*//***************************************************************************/
void ParticleObject::OnHitEnemy(HitResult& hit)
{
}

/**************************************************************************//**
	@brief		レンダリング
	@param[in]    rc	レンダリングコンテクスト
*//***************************************************************************/
void ParticleObject::RenderDX12(const RenderContextDX12& rc)
{
	m_particle->Render(rc);
}