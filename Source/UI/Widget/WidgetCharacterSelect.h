//! @file WidgetCharacterSelect.h
//! @note

#ifndef __INCLUDED_WIDGET_CHARACTER_SELECT_H__
#define __INCLUDED_WIDGET_CHARACTER_SELECT_H__

#include "UI/Widget/WidgetButtonImage.h"
#include "TAKOEngine/Rendering/Plane.h"
#include "PlayerCharacterData.h"

class SceneCharacter_E4C;

class WidgetCharacter : public WidgetButtonImage
{
public:
	WidgetCharacter(SceneCharacter_E4C* scene);
	WidgetCharacter(SceneCharacter_E4C* scene, int idx, PlayerCharacter* character);
	~WidgetCharacter();

	// 描画処理
	void Render(const RenderContext& rc);
	void RenderDX12(const RenderContextDX12& rc);
private:
	int m_idx = -1;
	SceneCharacter_E4C* m_pScene;
	PlayerCharacter* m_pCharacter;
};

/**************************************************************************//**
	@class	WidgetCharacterSelect
	@brief	キャラクター選択UI
	@par	[説明]
*//***************************************************************************/
class WidgetCharacterSelect : public Widget
{
public:
	WidgetCharacterSelect(SceneCharacter_E4C* scene);
	~WidgetCharacterSelect();

	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render(const RenderContext& rc);
	void RenderDX12(const RenderContextDX12& rc);
private:
	SceneCharacter_E4C* m_pScene;
	std::vector<WidgetCharacter*> m_pCharacterButtons;
};

#endif // !__INCLUDED_WIDGET_CHARACTER_SELECT_H__
