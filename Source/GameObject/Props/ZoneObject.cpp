#include "ZoneObject.h"

/**************************************************************************//**
	@brief    ÉÇÉfÉãê›íË
*//***************************************************************************/
ZoneObject::ZoneObject() : ModelObject()
{
	m_Effect = std::make_unique<ModelObject>("Data/Model/Object/EffectZone.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_Effect->SetShader("EffectZone", ModelShaderDX12Id::Zone);
	m_Effect->SetScale({ 0.0f, 0.0f, 0.0f });
}

/**************************************************************************//**
	@brief    çXêV
	@param[in]    elapsedTime
*//***************************************************************************/
void ZoneObject::Update(float elapsedTime)
{
	m_Effect->SetPosition(position);

	if (m_isVisibleActive)
	{
		m_currentActiveTime += elapsedTime;
		float progress;

		(m_currentActiveTime / m_timeToMaxZone) < 1.0f ? progress = (m_currentActiveTime / m_timeToMaxZone) : progress = 1.0f;

		if (m_currentZoneRadius < m_maxZoneRadius)
		{
			m_currentZoneRadius = m_maxZoneRadius * progress;

			m_Effect->SetScale({ m_currentZoneRadius , 1.0f , m_currentZoneRadius });
		}
		else
		{
			m_Effect->SetScale({ m_maxZoneRadius , 1.0f , m_maxZoneRadius });
		}

		m_Effect->Update(elapsedTime);

		m_activityTimer -= elapsedTime;

		if (m_activityTimer <= 0.0f)
		{
			m_isVisibleActive = false;
			m_currentActiveTime = 0.0f;
			m_currentZoneRadius = 0.0f;
			m_activityTimer = m_maxActivityTimer;
		}
	}
}

/**************************************************************************//**
	@brief    DX11ÇÃï`âÊ
	@param[in]    rc
*//***************************************************************************/
void ZoneObject::Render(const RenderContext& rc)
{
	if (m_isVisibleActive)
	{
		m_Effect->Render(rc);
	}
}

/**************************************************************************//**
	@brief    DX12ÇÃï`âÊ
	@param[in]    rc
*//***************************************************************************/
void ZoneObject::RenderDX12(const RenderContextDX12& rc)
{
	if (m_isVisibleActive)
	{
		m_Effect->RenderDX12(rc);
	}
}