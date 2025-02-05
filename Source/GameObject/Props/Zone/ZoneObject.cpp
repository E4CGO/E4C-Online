#include "ZoneObject.h"

#include "GameObject/Props/Zone/ZoneManager.h"

/**************************************************************************//**
	@brief    モデル設定
*//***************************************************************************/
ZoneObject::ZoneObject(GameObject* owner, float time, float radius) : ModelObject(), m_pOwner(owner), m_time(time), m_radius(radius)
{
	m_timer = m_time;
}

//ZoneObject::ZoneObject() : ModelObject()
//{
//	m_Effect = std::make_unique<ModelObject>("Data/Model/Object/EffectZone.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
//	m_Effect->SetShader("EffectZone", ModelShaderDX12Id::Zone);
//	m_Effect->SetScale({ 0.0f, 0.0f, 0.0f });
//}

/**************************************************************************//**
	@brief    更新
	@param[in]    elapsedTime
*//***************************************************************************/
void ZoneObject::Update(float elapsedTime)
{
	ModelObject::Update(elapsedTime);
	m_timer -= elapsedTime;
	if (m_timer <= 0.0f)
	{
		// 寿命による自滅
		m_timer = 0.0f;
		ZoneManager::Instance().Remove(this);
	}
}