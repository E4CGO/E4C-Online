//! @file DefenceBuffZone.h
//! @note 

#ifndef __INCLUDE_DEFENCE_BUFF_ZONE_H__
#define __INCLUDE_DEFENCE_BUFF_ZONE_H__

#include "ZoneObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

class DefenceBuffZone : public ZoneObject
{
public:
	DefenceBuffZone(PlayerCharacter* owner);
	~DefenceBuffZone() override {};

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;

protected:
	PlayerCharacter* m_pOwner;

	float m_maxZoneRadius = 5.0f;
	float m_ScaleTime = 2.0f;
	float m_ScaleTimer;

	std::unique_ptr<ModelObject> m_effect;
};

#endif // !__INCLUDE_DEFENCE_BUFF_ZONE_H__
