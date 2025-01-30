#include "HealingObject.h"
#include "Source/GameObject/Character/Player/PlayerCharacterManager.h"
#include "TAKOEngine/Tool/XMFLOAT.h"

/**************************************************************************//**
	@brief    モデル設定
*//***************************************************************************/
HealingObject::HealingObject() : ModelObject()
{
	m_EffectFirst = std::make_unique<ModelObject>("Data/Model/Object/EffectZone2.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_EffectFirst->SetShader("EffectZone2", ModelShaderDX12Id::HealCylinder);
	m_EffectFirst->SetScale({ 3.0f * m_size, 3.0f * m_size, 3.0f * m_size });

	m_EffectSecond = std::make_unique<ModelObject>("Data/Model/Object/EffectZone3.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_EffectSecond->SetShader("EffectZone3", ModelShaderDX12Id::HealCylinder);
	m_EffectSecond->SetScale({ 3.0f * m_size, 3.0f * m_size, 3.0f * m_size });

	m_EffectThird = std::make_unique<ModelObject>("Data/Model/Object/EffectCircle.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_EffectThird->SetShader("EffectCircle", ModelShaderDX12Id::HealCircle);
	m_EffectThird->SetScale({ 0.8f * m_size, 1.0f, 0.8f * m_size });
}

/**************************************************************************//**
	@brief    更新
	@param[in]    elapsedTime
*//***************************************************************************/
void HealingObject::Update(float elapsedTime)
{
	if (m_isVisibleActive)
	{
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
		if (XMFLOAT3HorizontalLength((player->GetPosition() - m_EffectThird->GetPosition())) <= 3.0f * m_size)
		{
			player->ModifyHp(10.0f * elapsedTime);
		}

		m_EffectFirst->Update(elapsedTime);
		m_EffectSecond->Update(elapsedTime);
		m_EffectThird->Update(elapsedTime);

		m_activityTimer -= elapsedTime;

		if (m_activityTimer <= 0.0f)
		{
			m_isVisibleActive = false;
			m_activityTimer = m_maxActivityTimer;
		}
	}
}

/**************************************************************************//**
	@brief    DX11の描画
	@param[in]    rc
*//***************************************************************************/
void HealingObject::Render(const RenderContext& rc)
{
	if (m_isVisibleActive)
	{
		m_EffectFirst->Render(rc);
		m_EffectSecond->Render(rc);
		m_EffectThird->Render(rc);
	}
}

/**************************************************************************//**
	@brief    DX12の描画
	@param[in]    rc
*//***************************************************************************/
void HealingObject::RenderDX12(const RenderContextDX12& rc)
{
	if (m_isVisibleActive)
	{
		m_EffectThird->RenderDX12(rc);
		m_EffectFirst->RenderDX12(rc);
		m_EffectSecond->RenderDX12(rc);
	}
}

void HealingObject::SetObjectPositions(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 front)
{
	DirectX::XMFLOAT3 startPosition = position;
	DirectX::XMVECTOR vStartPosition = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));
	direction = DirectX::XMVectorScale(direction, m_distance);
	DirectX::XMVECTOR newPosition = XMVectorAdd(vStartPosition, direction);
	DirectX::XMStoreFloat3(&startPosition, newPosition);

	m_EffectFirst->SetPosition(startPosition);
	m_EffectSecond->SetPosition(startPosition);
	m_EffectThird->SetPosition(startPosition);
}