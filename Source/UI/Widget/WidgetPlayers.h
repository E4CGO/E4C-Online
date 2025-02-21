//! @file WidgetPlayers.h
//! @note 

#ifndef __INCLUDED_WIDGET_PLAYERS_H__
#define __INCLUDED_WIDGET_PLAYERS_H__

#include "TAKOEngine/GUI/Widget.h"
#include "WidgetImage.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

/**************************************************************************//**
	@class	WidgetPlayers
	@brief	他プレイヤーを表示
	@par    [説明]
*//***************************************************************************/
class WidgetPlayers : public Widget
{
public:
	WidgetPlayers();
	~WidgetPlayers();

	// 描画処理
	void Render(const RenderContext& rc) override {};
	void RenderDX12(const RenderContextDX12& rc) override;
private:
	const static int MAX_PLAYER = 4;
	// SpriteDX12が再利用できないため複数生成
	// TODO：SpriteDX12の完成
	WidgetImage* m_pMaleIcons[WidgetPlayers::MAX_PLAYER];
	WidgetImage* m_pFemaleIcons[WidgetPlayers::MAX_PLAYER];

};

#endif // __INCLUDED_WIDGET_PLAYERS_H__