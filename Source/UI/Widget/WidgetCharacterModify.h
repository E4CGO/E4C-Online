//! @file WidgetCharacterModify.h
//! @note 

#ifndef __INCLUDED_WIDGET_CHARACTER_MODIFY_H__
#define __INCLUDED_WIDGET_CHARACTER_MODIFY_H__

#include "TAKOEngine/GUI/Widget.h"
#include "PlayerCharacterData.h"

class SceneCharacter_E4C;
/**************************************************************************//**
	@class	WidgetCharacterModify
	@brief	キャラクター編集UI
	@par	[説明]
*//***************************************************************************/
class WidgetCharacterModify : public Widget
{
public:
	WidgetCharacterModify(SceneCharacter_E4C* scene);
	~WidgetCharacterModify();

	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render(const RenderContext& rc);

	void RenderDX12(const RenderContextDX12& rc);

	void SaveData();
private:
	SceneCharacter_E4C* m_pScene;
	PlayerCharacterData::CharacterInfo m_info;
	PlayerCharacterData::CharacterInfo m_infoTemp;
	std::vector<Widget*> m_pWidgets;
};


#endif // !__INCLUDED_WIDGET_CHARACTER_MODIFY_H__