//! @file ChargeObject.h
//! @note

#ifndef __INCLUDE_CHARGE_OBJECT_H__
#define __INCLUDE_CHARGE_OBJECT_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	ChargeObject
	@brief	Chargeの効果オブジェクトクラス
	@par	[説明]
		Chargeの描画
*//***************************************************************************/
class ChargeObject : public ModelObject
{
public:
	ChargeObject();
	virtual ~ChargeObject() = default;

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	const bool GetActivity() const { return m_isVisibleActive; }
	void SetLooping() { this->isLooping = true; }

	void Activate() {
		if (!m_isVisibleActive)
		{
			this->m_isVisibleActive = true;
		}
	}

private:

	const float m_maxActivityTimer = 0.8f;
	const float m_Delay = -0.02f;

	float m_activityTimer = m_maxActivityTimer;
	float m_maxZoneRadius = 1.0f;
	float m_currentZoneRadiusFirst = 0.0f;
	float m_currentZoneRadiusSecond = 0.0f;

	bool m_isVisibleActive = false;

	float m_timeToMaxZone = 0.1f;
	float m_currentActiveTimeFirst = 0.0f;
	float m_currentActiveTimeSecond = m_Delay;

	bool isLooping = false;

	std::unique_ptr<ModelObject> m_EffectFirst;
	std::unique_ptr<ModelObject> m_EffectSecond;
};

#endif // !__INCLUDE_CHARGE_OBJECT_H__
