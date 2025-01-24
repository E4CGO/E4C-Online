//! @file WidgetPlayerHP.h
//! @note

#ifndef __INCLUDED_WIDGET_PLAYER_HP__
#define __INCLUDED_WIDGET_PLAYER_HP__

#include <memory>

#include "TAKOEngine/GUI/Widget.h"

#include "GameObject/Character/Player/PlayerCharacter.h"

/**************************************************************************//**
	@class	WidgetPlayerHP
	@brief	プレイヤーのHP、MP、SPを表示
	@par    [説明]
*//***************************************************************************/
class WidgetPlayerHP : public Widget
{
public:
	WidgetPlayerHP();
	virtual ~WidgetPlayerHP() = default;

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
	void RenderDX12(const RenderContextDX12& rc) override;
private:
	PlayerCharacter* player = nullptr;
	std::shared_ptr<Sprite> gauge;
	std::shared_ptr<Sprite> gaugeFrame;

	std::shared_ptr<SpriteDX12> HPgaugeDX12;
	std::shared_ptr<SpriteDX12> HPgaugeFrameDX12;
	std::shared_ptr<SpriteDX12> MPgaugeDX12;
	std::shared_ptr<SpriteDX12> SPgaugeDX12;
	std::shared_ptr<SpriteDX12> SPMPgaugeFrameDX12;

	DirectX::XMFLOAT4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 描画用
	float tempHp;
};

#endif // !__INCLUDED_WIDGET_PLAYER_HP__