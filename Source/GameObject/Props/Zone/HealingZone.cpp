//! @file HealingZone.cpp
//! @note 

#include "HealingZone.h"
#include "Source/GameObject/Character/Player/PlayerCharacterManager.h"
//#include "TAKOEngine/Tool/XMFLOAT.h"

/**************************************************************************//**
	@brief    モデル設定
*//***************************************************************************/
HealingZone::HealingZone(PlayerCharacter* owner, float time, float radius) : ZoneObject(owner, time, radius)
{
	m_effect[0] = std::make_unique<ModelObject>("Data/Model/Object/EffectZone2.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect[0]->SetShader("EffectZone2", ModelShaderDX12Id::HealCylinder);
	m_effect[0]->SetScale({ 3.0f * m_radius, 3.0f * m_radius, 3.0f * m_radius });

	m_effect[1] = std::make_unique<ModelObject>("Data/Model/Object/EffectZone3.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect[1]->SetShader("EffectZone3", ModelShaderDX12Id::HealCylinder);
	m_effect[1]->SetScale({ 3.0f * m_radius, 3.0f * m_radius, 3.0f * m_radius });

	m_effect[2] = std::make_unique<ModelObject>("Data/Model/Object/EffectCircle.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect[2]->SetShader("EffectCircle", ModelShaderDX12Id::HealCircle);
	m_effect[2]->SetScale({ 0.8f * m_radius, 1.0f, 0.8f * m_radius });
}

/**************************************************************************//**
	@brief    更新
	@param[in]    elapsedTime
*//***************************************************************************/
void HealingZone::Update(float elapsedTime)
{
	ZoneObject::Update(elapsedTime);
	for (const std::unique_ptr<ModelObject>& effect : m_effect)
	{
		effect->SetPosition(position);
		effect->Update(elapsedTime);
	}

	m_healingTimer += elapsedTime;
	if (m_healingTimer > m_healingTime)
	{
		m_healingTimer = m_healingTime;
		PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
		if (XMFLOAT3HorizontalLengthSq((player->GetPosition() - position)) <= 3.0f * m_radius)
		{
			player->ModifyHp(1);
			m_healingTimer = 0.0f;
		}
	}
}

/**************************************************************************//**
	@brief    DX11の描画
	@param[in]    rc
*//***************************************************************************/
void HealingZone::Render(const RenderContext& rc)
{
	for (const std::unique_ptr<ModelObject>& effect : m_effect)
	{
		effect->Render(rc);
	}
}

/**************************************************************************//**
	@brief    DX12の描画
	@param[in]    rc
*//***************************************************************************/
void HealingZone::RenderDX12(const RenderContextDX12& rc)
{
	for (const std::unique_ptr<ModelObject>& effect : m_effect)
	{
		effect->RenderDX12(rc);
	}
}