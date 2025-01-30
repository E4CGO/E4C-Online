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

	void SetCharacterOptions();
private:

	DirectX::XMFLOAT2 pos;

	const char* labels[13] = {
		"性別",
		"髪",
		"髪の色",
		//"目の色",
		"靴",
		"肌の色",
		"上半身服",
		"上半身服色",
		"下半身服",
		"下半身服色",
		"腕装備",
		"腕装備色",
		"左手装備",
		"右手装備"
	};

	SceneCharacter_E4C* m_pScene;
	PlayerCharacterData::CharacterInfo m_info;
	PlayerCharacterData::CharacterInfo m_infoTemp;
	std::vector<Widget*> m_pWidgets;
	std::vector<Widget*> m_pWidgetsCharacterOptions;

};

#endif // !__INCLUDED_WIDGET_CHARACTER_MODIFY_H__