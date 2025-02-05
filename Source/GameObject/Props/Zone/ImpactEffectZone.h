//! @file ImpactEffectZone.h
//! @note 


#ifndef __INCLUDE_IMPACT_EFFECT_ZONE_H__
#define __INCLUDE_IMPACT_EFFECT_ZONE_H__

#include "ZoneObject.h"


/**************************************************************************//**
	@class	HealingZone
	@brief	Healingの効果オブジェクトクラス
	@par	[説明]
		HealingZoneの描画
*//***************************************************************************/
class ImpactEffectZone : public ZoneObject
{
public:
	ImpactEffectZone(GameObject* owner, float time = 0.5f, float radius = 5.0f);
	virtual ~ImpactEffectZone() override {};

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;
private:
	float m_speed = 0.1f;
	float m_delay = 0.1f;

	float m_effectRadius[2] = { 0.0f, 0.0f };
	std::unique_ptr<ModelObject> m_effect[2];
};

#endif // !__INCLUDE_IMPACT_EFFECT_ZONE_H__
