//! @file ZoneObject.h
//! @note

#ifndef __INCLUDE_ZONE_OBJECT_H__
#define __INCLUDE_ZONE_OBJECT_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	ZoneObject
	@brief	Zoneの効果オブジェクトクラス
	@par	[説明]
		Zoneの描画
*//***************************************************************************/
class ZoneObject : public ModelObject
{
public:
	ZoneObject();
	virtual ~ZoneObject() = default;

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	const bool GetActivity() const { return m_isVisibleActive; }
	void Activate() {
		if (!m_isVisibleActive)
		{
			this->m_isVisibleActive = true;
		}
	}

private:

	const float m_maxActivityTimer = 10.0f;
	float m_activityTimer = m_maxActivityTimer;
	float m_maxZoneRadius = 5.0f;
	float m_currentZoneRadius = 0.0f;

	bool m_isVisibleActive = false;

	float m_timeToMaxZone = 2.0f;
	float m_currentActiveTime = 0.0f;

	std::unique_ptr<ModelObject> m_Effect;
};

#endif // !__INCLUDE_ZONE_OBJECT_H__
