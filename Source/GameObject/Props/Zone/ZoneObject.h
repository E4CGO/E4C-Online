//! @file ZoneObject.h
//! @note

#ifndef __INCLUDE_ZONE_OBJECT_H__
#define __INCLUDE_ZONE_OBJECT_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	ZoneObject
	@brief	Zoneの効果オブジェクト基底クラス
	@par	[説明]
		Zoneの描画
*//***************************************************************************/
class ZoneObject : public ModelObject
{
public:
	ZoneObject(GameObject* owner = nullptr, float time = 10.0f, float radius = 3.0f);
	virtual ~ZoneObject() = default;

	void SetTime(float second) { m_time = m_timer = second; }

	virtual void Update(float elapsedTime);
	virtual void Render(const RenderContext& rc) = 0;
	virtual void RenderDX12(const RenderContextDX12& rc) = 0;

protected:
	GameObject* m_pOwner;
	float m_time = 10.0f;
	float m_timer;
	float m_radius;
};

#endif // !__INCLUDE_ZONE_OBJECT_H__
