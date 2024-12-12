//! @file WidgetCharacterSelect.cpp
//! @note

#include "WidgetCharacterSelect.h"
#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"
#include "PlayerCharacterData.h"

/**************************************************************************//**
	@brief	コンストラクタ
	@param[in]	scene	キャラクター選択シーン参照ポインタ
	@return なし
*//***************************************************************************/
WidgetCharacterSelect::WidgetCharacterSelect(SceneCharacter_E4C* scene) : m_pScene(scene)
{
	const std::vector<PlayerCharacter*>& characters = m_pScene->GetCharacters();

	DirectX::XMFLOAT2 size = { SCREEN_W * 0.2f, SCREEN_H * 0.7f };
	for (int i = 0; i < characters.size(); i++)
	{
		WidgetCharacter* widget = new WidgetCharacter(scene, i, characters.at(i));
		widget->SetSize(size);
		m_pCharacterButtons.push_back(widget);
	}
	WidgetCharacter* widget = new WidgetCharacter(scene);
	widget->SetSize(size);
	m_pCharacterButtons.push_back(widget);
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime 経過時間
	@return		なし
*//***************************************************************************/
void WidgetCharacterSelect::Update(float elapsedTime)
{
	const std::vector<PlayerCharacter*>& characters = m_pScene->GetCharacters();

	float posX = 3.5f;
	for (int i = 0; i < characters.size(); i++)
	{
		DirectX::XMFLOAT3 position = characters.at(i)->GetScreenPosition();
		m_pCharacterButtons[i]->SetPosition({ position.x - m_pCharacterButtons[i]->GetSize().x * 0.5f, SCREEN_H * 0.1f });
		posX -= 3.5;
	}
	posX = T_GRAPHICS.GetScreenPosition({ posX, 0.0f, 5.0f }).x;
	m_pCharacterButtons[characters.size()]->SetPosition({ posX - m_pCharacterButtons[characters.size()]->GetSize().x * 0.5f, SCREEN_H * 0.1f });

	for (WidgetCharacter* button : m_pCharacterButtons)
	{
		button->Update(elapsedTime);
	}
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetCharacterSelect::Render(const RenderContext& rc)
{
	for (WidgetCharacter* button : m_pCharacterButtons)
	{
		button->Render(rc);
	}
}

/**************************************************************************//**
	@brief		DX12描画処理
	@param[in]	rc レンダーコンテンツ
	@return		なし
*//***************************************************************************/
void WidgetCharacterSelect::RenderDX12(const RenderContextDX12& rc)
{
	for (WidgetCharacter* button : m_pCharacterButtons)
	{
		button->RenderDX12(rc);
	}
}
/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
WidgetCharacterSelect::~WidgetCharacterSelect()
{
	for (const WidgetCharacter* button : m_pCharacterButtons)
	{
		delete button;
	}
	m_pCharacterButtons.clear();
}

/**************************************************************************//**
	@brief		コンストラク
	@param[in]	scene キャラクター選択シーン参照ポインタ
	@return		なし
*//***************************************************************************/
WidgetCharacter::WidgetCharacter(SceneCharacter_E4C* scene) : m_pScene(scene), m_pCharacter(nullptr), WidgetButtonImage("+", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
	PlayerCharacter* newCharacter = new PlayerCharacter({ "", {} });
	newCharacter->SetPosition({ 3.5f - 3.5f * (m_pScene->GetCharacters().size()), 0.0f, 5.0f });
	newCharacter->SetKinematic(true);
	m_pScene->RegisterCharacter(newCharacter);
	m_pScene->SetSelectedCharacter(newCharacter);
	PLAYER_CHARACTER_DATA.SetCharacterInfo(m_pScene->GetSelectedCharacterIdx(), {});

	m_pScene->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTER_CREATION);
	})
{
	m_color = { 0.0f, 0.0f, 0.0f, 0.5f };
}

/**************************************************************************//**
	@brief		コンストラク
	@param[in]	scene		キャラクター選択シーン参照ポインタ
	@param[in]	idx			キャラクターインデックス
	@param[in]	character	キャラクター参照ポインタ
*//***************************************************************************/
WidgetCharacter::WidgetCharacter(SceneCharacter_E4C* scene, int idx, PlayerCharacter* character) : m_pScene(scene), m_pCharacter(character), m_idx(idx), WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
	m_pScene->SetSelectedCharacter(m_idx);
	m_pScene->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTER_CREATION);
	})
{
	m_color = { 0.0f, 0.0f, 0.0f, 0.5f };
}

/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
WidgetCharacter::~WidgetCharacter()
{
}