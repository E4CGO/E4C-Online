//! @file WidgetCharacterModify.cpp
//! @note 
#include "WidgetCharacterModify.h"
#include <unordered_map>
#include <cstring>

#include "WidgetInputInt.h"

#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"
/**************************************************************************//**
 	@brief		コンストラク
	@param[in]	scene	キャラクターシーン参照ポインタ
*//***************************************************************************/
WidgetCharacterModify::WidgetCharacterModify(SceneCharacter_E4C* scene) : m_pScene(scene)
{
	//m_pScene->GetSelectedCharacter();
	m_info = PLAYER_CHARACTER_DATA.GetCharacterInfo(m_pScene->GetSelectedCharacterIdx());
	m_infoTemp = m_info;

	m_pWidgets.clear();

	DirectX::XMFLOAT2 pos = { SCREEN_W * 0.6f, SCREEN_H * 0.1f };

	const char* labels[] = {
		"性別",
		"髪",
		"髪の色",
		"目の色",
		"肌の色",
		"上半身服",
		"上半身服色",
		"下半身服",
		"下半身服色",
		"腕装備",
		"腕装備色",
		"左手装備",
		"右手装備",
	};


	for (uint8_t i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
	{
		uint8_t size = static_cast<uint8_t>(PLAYER_CHARACTER_DATA.GetAppearancePatternSize(i));
		if (size > 1)
		{
			WidgetText* labelWidget = new WidgetText(labels[i]);
			labelWidget->SetPosition(pos);
			pos.y += labelWidget->GetSize().y;
			m_pWidgets.push_back(labelWidget);

			WidgetInputInt<uint8_t>* widget = new WidgetInputInt<uint8_t>(&m_info.pattern[i], 0, size - 1);
			widget->SetPosition(pos);
			pos.y += widget->GetSize().y;
			m_pWidgets.push_back(widget);
		}
	}
}
/**************************************************************************//**
 	@brief		更新処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void WidgetCharacterModify::Update(float elapsedTime)
{
	for (Widget* widget : m_pWidgets)
	{
		widget->Update(elapsedTime);
	}

	if (std::memcmp(m_info.pattern, m_infoTemp.pattern, sizeof(m_info.pattern)) != 0) // 更新あり
	{
		m_pScene->GetSelectedCharacter()->LoadAppearance(m_info.pattern);
		m_infoTemp = m_info;
	}

}
/**************************************************************************//**
 	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ
*//***************************************************************************/
void WidgetCharacterModify::Render(const RenderContext& rc)
{
	for (Widget* widget : m_pWidgets)
	{
		widget->Render(rc);
	}
}
/**************************************************************************//**
 	@brief		DX12描画処理
	@param[in]	rc	レンダーコンテンツ
*//***************************************************************************/
void WidgetCharacterModify::RenderDX12(const RenderContextDX12& rc)
{
	for (Widget* widget : m_pWidgets)
	{
		widget->RenderDX12(rc);
	}
}
/**************************************************************************//**
 	@brief	編集したデータを保存する
*//***************************************************************************/
void WidgetCharacterModify::SaveData()
{
	PLAYER_CHARACTER_DATA.SetCharacterInfo(
		m_pScene->GetSelectedCharacterIdx(),
		m_info
	);
}
/**************************************************************************//**
 	@brief	デストラクタ
*//***************************************************************************/
WidgetCharacterModify::~WidgetCharacterModify()
{
	for (Widget* widget : m_pWidgets)
	{
		delete widget;
	}
	m_pWidgets.clear();
}
