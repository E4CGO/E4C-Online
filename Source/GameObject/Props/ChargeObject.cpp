#include "ChargeObject.h"

/**************************************************************************//**
	@brief    モデル設定
*//***************************************************************************/
ChargeObject::ChargeObject() : ModelObject()
{
	m_EffectFirst = std::make_unique<ModelObject>("Data/Model/Object/EffectCone.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_EffectFirst->SetShader("EffectCone", ModelShaderDX12Id::Charge);
	m_EffectFirst->SetScale({ 0.0f, 0.0f, 0.0f });

	m_EffectSecond = std::make_unique<ModelObject>("Data/Model/Object/EffectCone2.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_EffectSecond->SetShader("EffectCone2", ModelShaderDX12Id::Charge);
	m_EffectSecond->SetScale({ 0.0f, 0.0f, 0.0f });
}

/**************************************************************************//**
	@brief    更新
	@param[in]    elapsedTime
*//***************************************************************************/
void ChargeObject::Update(float elapsedTime)
{
	m_EffectFirst->SetPosition(position);

	if (m_isVisibleActive)
	{
		m_currentActiveTimeFirst += elapsedTime;
		m_currentActiveTimeSecond += elapsedTime;
		float progressFirst;
		float progressSecond;

		(m_currentActiveTimeFirst / m_timeToMaxZone) < 1.0f ? progressFirst = (m_currentActiveTimeFirst / m_timeToMaxZone) : progressFirst = 1.0f;
		(m_currentActiveTimeSecond / m_timeToMaxZone) < 1.0f ? progressSecond = (m_currentActiveTimeSecond / m_timeToMaxZone) : progressSecond = 1.0f;

		if (isLooping)
		{
			if (m_currentZoneRadiusFirst < m_maxZoneRadius)
			{
				m_currentZoneRadiusFirst = m_maxZoneRadius * progressFirst;

				m_EffectFirst->SetScale({ m_currentZoneRadiusFirst , 1.0f , m_currentZoneRadiusFirst });
			}
			else
			{
				m_currentActiveTimeFirst = 0.0f;
				m_currentZoneRadiusFirst = 0.0f;
			}

			if (m_currentZoneRadiusSecond < m_maxZoneRadius)
			{
				m_currentZoneRadiusSecond = m_maxZoneRadius * progressSecond;

				m_EffectSecond->SetScale({ m_currentZoneRadiusFirst , 1.0f , m_currentZoneRadiusFirst });
			}
			else
			{
				m_currentActiveTimeSecond = 0.0f;
				m_currentZoneRadiusSecond = 0.0f;
			}
		}
		else
		{
			if (m_currentZoneRadiusFirst < m_maxZoneRadius)
			{
				m_currentZoneRadiusFirst = m_maxZoneRadius * progressFirst;

				m_EffectFirst->SetScale({ m_currentZoneRadiusFirst , 1.0f , m_currentZoneRadiusFirst });
			}
			else
			{
				m_EffectFirst->SetScale({ m_maxZoneRadius , 1.0f , m_maxZoneRadius });
			}

			if (m_currentZoneRadiusSecond < m_maxZoneRadius)
			{
				m_currentZoneRadiusSecond = m_maxZoneRadius * progressSecond;

				m_EffectSecond->SetScale({ m_currentZoneRadiusFirst , 1.0f , m_currentZoneRadiusFirst });
			}
			else
			{
				m_EffectSecond->SetScale({ m_maxZoneRadius , 1.0f , m_maxZoneRadius });
			}
		}

		m_EffectFirst->Update(elapsedTime);
		m_EffectSecond->Update(elapsedTime);

		m_activityTimer -= elapsedTime;

		if (m_activityTimer <= 0.0f)
		{
			m_isVisibleActive = false;
			m_currentActiveTimeFirst = 0.0f;
			m_currentActiveTimeSecond = m_Delay;
			m_currentZoneRadiusFirst = 0.0f;
			m_currentZoneRadiusSecond = 0.0f;
			m_activityTimer = m_maxActivityTimer;
		}
	}
}

/**************************************************************************//**
	@brief    DX11の描画
	@param[in]    rc
*//***************************************************************************/
void ChargeObject::Render(const RenderContext& rc)
{
	if (m_isVisibleActive)
	{
		m_EffectFirst->Render(rc);
		m_EffectSecond->Render(rc);
	}
}

/**************************************************************************//**
	@brief    DX12の描画
	@param[in]    rc
*//***************************************************************************/
void ChargeObject::RenderDX12(const RenderContextDX12& rc)
{
	if (m_isVisibleActive)
	{
		m_EffectFirst->RenderDX12(rc);
		m_EffectSecond->RenderDX12(rc);
	}
}