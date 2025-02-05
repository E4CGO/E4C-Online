//! @file HealingObject.h
//! @note

#ifndef __INCLUDE_HEALING_OBJECT_H__
#define __INCLUDE_HEALING_OBJECT_H__

#include "GameObject/Props/Zone/ZoneObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
	@class	HealingZone
	@brief	Healingの効果オブジェクトクラス
	@par	[説明]
		HealingZoneの描画
*//***************************************************************************/
class HealingZone : public ZoneObject
{
public:
	HealingZone(PlayerCharacter* owner, float time = 10.0f,  float radius = 1.0f);
	virtual ~HealingZone() override {};

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;
private:
	PlayerCharacter* m_pOwner;

	float m_healingTime = 0.1f; // 治療の頻度
	float m_healingTimer = 0.0f;

	std::unique_ptr<ModelObject> m_effect[3];
};

#endif // !__INCLUDE_HEALING_OBJECT_H__
