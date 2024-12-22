#include "SceneCharacter_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "PlayerCharacterData.h"

#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

using namespace SceneCharacter_E4CState;

/**************************************************************************//**
	@brief    データーを読み込みまたはデーターを初期化
*//***************************************************************************/
void SceneCharacter_E4CState::InitState::Enter()
{
	PLAYER_CHARACTER_DATA.LoadData();

	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 0.0f, 1.5f, 12.4f }, { 0.0f, 1.5f, 0.73f }, { 0.0f, 1.0f, 0.0f }
	);

	DirectX::XMFLOAT3 pos = { 3.5f, 0.0f, 5.0f };
	const std::vector<PlayerCharacterData::CharacterInfo>& infos = PLAYER_CHARACTER_DATA.GetCharacterInfosData();
	for (const PlayerCharacterData::CharacterInfo& info : infos)
	{
		PlayerCharacter* character = new PlayerCharacter(info);
		character->SetPosition(pos);
		character->SetKinematic(true);
		pos.x -= 3.5f;
		owner->RegisterCharacter(character);
	}

	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTER_SELECTION);
}

/**************************************************************************//**
	@brief
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4CState::InitState::Execute(float elapsedTime)
{
}

/**************************************************************************//**
	@brief	終わり
*//***************************************************************************/
void SceneCharacter_E4CState::InitState::Exit()
{

}

/**************************************************************************//**
	@brief	全てキャラを描画と選択
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterSelectionState::Enter()
{
	m_pWidgetCharacterSelect = new WidgetCharacterSelect(owner);
	UI.Register(m_pWidgetCharacterSelect);
	for (PlayerCharacter* character : owner->GetCharacters())
	{
		character->Show();
	}

	m_cameraOriginPos = CameraManager::Instance().GetCamera()->GetEye();
	m_cameraOriginFocus = CameraManager::Instance().GetCamera()->GetFocus();
	m_cameraTimer = 0.0f;
}
/**************************************************************************//**
	@brief		実行
	@param[in]    elapsedTime
*//***************************************************************************/

void SceneCharacter_E4CState::CharacterSelectionState::Execute(float elapsedTime)
{
	Camera* camera = CameraManager::Instance().GetCamera();

	if (m_cameraTimer < m_cameraTime)
	{
		m_cameraTimer += elapsedTime;
		if (m_cameraTimer > m_cameraTime) m_cameraTimer = m_cameraTime;
		camera->Move2PointToCamera(
			m_cameraOriginPos,
			{ 0.0f, 1.5f, 12.4f },
			m_cameraOriginFocus,
			{ 0.0f, 1.5f, 0.73f },
			m_cameraTimer,
			m_cameraTime
		);
	}

	for (PlayerCharacter* character : owner->GetCharacters())
	{
		character->FaceTo(camera->GetEye());
	}
}
/**************************************************************************//**
	@brief	終わり
*//***************************************************************************/

void SceneCharacter_E4CState::CharacterSelectionState::Exit()
{
	UI.Remove(m_pWidgetCharacterSelect);
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//**
	@brief	キャラクリ
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Enter()
{
	m_pCharacter = owner->GetSelectedCharacter();
	std::vector<PlayerCharacter*> characters = owner->GetCharacters();
	for (PlayerCharacter* character : characters)
	{
		character->Hide();
	}
	m_pCharacter->Show();

	m_cameraOriginPos = CameraManager::Instance().GetCamera()->GetEye();
	m_cameraOriginFocus = CameraManager::Instance().GetCamera()->GetFocus();
	m_cameraTimer = 0.0f;

	m_pBackBtn = new WidgetButtonText("Back", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTER_SELECTION);
		});
	m_pBackBtn->SetPosition({ SCREEN_H * 0.1f, SCREEN_H * 0.1f });
	UI.Register(m_pBackBtn);

	m_pStartBtn = new WidgetButtonText("Start", [&](WidgetButton*) {
		PLAYER_CHARACTER_DATA.SetCurrentCharacter(owner->GetSelectedCharacterIdx());
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	m_pStartBtn->SetPosition({ SCREEN_W * 0.5f - (m_pStartBtn->GetSize().x * 0.5f), SCREEN_H * 0.8f });
	UI.Register(m_pStartBtn);

	m_pWidgetCharacterModify = new WidgetCharacterModify(owner);
	UI.Register(m_pWidgetCharacterModify);
}

/**************************************************************************//**
	@brief		キャラ設定を変わる
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Execute(float elapsedTime)
{
	Camera* camera = CameraManager::Instance().GetCamera();

	if (m_cameraTimer < m_cameraTime)
	{
		m_cameraTimer += elapsedTime;
		if (m_cameraTimer > m_cameraTime) m_cameraTimer = m_cameraTime;
		camera->Move2PointToCamera(
			m_cameraOriginPos,
			m_pCharacter->GetPosition() + DirectX::XMFLOAT3{ -0.5f, 2.5f, 5.5f },
			m_cameraOriginFocus,
			m_pCharacter->GetPosition() + DirectX::XMFLOAT3{ -0.5f, 1.2f, 0.0f },
			m_cameraTimer,
			m_cameraTime
		);
	}

	m_pCharacter->FaceTo(camera->GetEye());
}
/**************************************************************************//**
	@brief	終わり、データーをPLAYER_CHARACTER_DATAとJSONにセーブ
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Exit()
{
	UI.Remove(m_pBackBtn);
	m_pBackBtn = nullptr;

	UI.Remove(m_pStartBtn);
	m_pStartBtn = nullptr;

	m_pWidgetCharacterModify->SaveData();
	UI.Remove(m_pWidgetCharacterModify);
	m_pWidgetCharacterModify = nullptr;

	PLAYER_CHARACTER_DATA.SaveData();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//**
	@brief	ゲームを始まり
*//***************************************************************************/
void SceneCharacter_E4CState::StartState::Enter()
{
}

/**************************************************************************//**
	@brief		シーンゲームに移動
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4CState::StartState::Execute(float elapsedTime)
{
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame_E4C));
}

/**************************************************************************//*
	@brief	終わり
*//***************************************************************************/
void SceneCharacter_E4CState::StartState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}