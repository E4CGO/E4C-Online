//! @file HealingObject.h
//! @note

#ifndef __INCLUDE_HEALING_OBJECT_H__
#define __INCLUDE_HEALING_OBJECT_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	HealingObject
	@brief	Healingの効果オブジェクトクラス
	@par	[説明]
		HealingZoneの描画
*//***************************************************************************/
class HealingObject : public ModelObject
{
public:
	HealingObject();
	virtual ~HealingObject() = default;

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	void SetObjectPositions(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 front);

	const bool GetActivity() const { return m_isVisibleActive; }
	void Activate() {
		if (!m_isVisibleActive)
		{
			this->m_isVisibleActive = true;
		}
	}

private:

	const float m_size = 1.0f;
	const float m_maxActivityTimer = 10.0f;
	float m_activityTimer = m_maxActivityTimer;
	float m_healingTimer = 0.0f;
	float m_maxZoneRadius = 5.0f;

	bool m_isVisibleActive = false;

	float m_distance = 5.0f;

	std::unique_ptr<ModelObject> m_EffectFirst;
	std::unique_ptr<ModelObject> m_EffectSecond;
	std::unique_ptr<ModelObject> m_EffectThird;
};

#endif // !__INCLUDE_HEALING_OBJECT_H__
