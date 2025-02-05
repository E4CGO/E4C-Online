//! @file ImpactEffectZone.cpp
//! @note 

#include "ImpactEffectZone.h"

ImpactEffectZone::ImpactEffectZone(GameObject* owner, float time, float radius) : ZoneObject(owner, time, radius)
{
	m_speed = radius / time;


	m_effect[0] = std::make_unique<ModelObject>("Data/Model/Object/EffectCone.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect[0]->SetShader("EffectCone", ModelShaderDX12Id::Charge);
	m_effect[0]->SetScale({ 0.0f, 0.0f, 0.0f });

	m_effect[1] = std::make_unique<ModelObject>("Data/Model/Object/EffectCone2.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect[1]->SetShader("EffectCone2", ModelShaderDX12Id::Charge);
	m_effect[1]->SetScale({ 0.0f, 0.0f, 0.0f });
}

void ImpactEffectZone::Update(float elapsedTime)
{
	ZoneObject::Update(elapsedTime);
	if (m_timer <= 0.0f) return;

	m_effectRadius[0] += m_speed * elapsedTime;
	while (m_effectRadius[0] > m_radius)
	{
		m_effectRadius[0] -= m_radius;
	}

	if (m_timer < (m_time - m_delay))
	{
		m_effectRadius[1] += m_speed * elapsedTime;
		while (m_effectRadius[1] > m_radius)
		{
			m_effectRadius[1] -= m_radius;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		m_effect[i]->SetPosition(position);
		m_effect[i]->SetScale({ m_effectRadius[i], 1.0f, m_effectRadius[i] });
		m_effect[i]->Update(elapsedTime);
	}
}
void ImpactEffectZone::Render(const RenderContext& rc)
{
	for (const std::unique_ptr<ModelObject>& effect : m_effect)
	{
		effect->Render(rc);
	}
}
void ImpactEffectZone::RenderDX12(const RenderContextDX12& rc)
{
	for (const std::unique_ptr<ModelObject>& effect : m_effect)
	{
		effect->RenderDX12(rc);
	}
}
