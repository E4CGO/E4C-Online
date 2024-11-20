#include "SceneCharacter_E4CState.h"

#include <fstream>

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "PlayerCharacterData.h"

#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

#include "Source/UI/Widget/WidgetButton.h"
#include "Source/UI/Widget/WidgetDragFloat.h"
#include "Source/UI/Widget/WidgetUpDown.h"

using namespace SceneCharacter_E4CState;

inline bool FileExist(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

/**************************************************************************//**
	@brief    データーを読み込みまたはデーターを初期化
*//***************************************************************************/
void SceneCharacter_E4CState::InitState::Enter()
{
	if (!FileExist("CharacterInfos.json"))
	{
		nlohmann::json newSave;

		newSave[0]["Characters"] = {
			{
				{"Save", ""},
				{"Visible", true},
				{"Character", {
					{"GENDER", 0},
					{ "HAIR", 0 },
					{"HAIR_COLOR", 0},
					{"EYE_COLOR", 0},
					{"SKIN_COLOR", 0},
					{"TOP", 0},
					{"TOP_COLOR", 0},
					{"BOTTOM", 0},
					{"BOTTOM_COLOR", 0},
					{"ARM_GEAR", 0},
					{"ARM_GEAR_COLOR", 0},
					{"LEFT_HAND_EQUIPMENT", 0},
					{"RIGHT_HAND_EQUIPMENT", 0},
				}}
			},
			{
				{"Save", ""},
				{"Visible", true},
				{"Character", {
					{"GENDER", 0},
					{ "HAIR", 0 },
					{"HAIR_COLOR", 0},
					{"EYE_COLOR", 0},
					{"SKIN_COLOR", 0},
					{"TOP", 0},
					{"TOP_COLOR", 0},
					{"BOTTOM", 0},
					{"BOTTOM_COLOR", 0},
					{"ARM_GEAR", 0},
					{"ARM_GEAR_COLOR", 0},
					{"LEFT_HAND_EQUIPMENT", 0},
					{"RIGHT_HAND_EQUIPMENT", 0},
				}}
			},
			{
				{"Save", ""},
				{"Visible", true},
				{"Character", {
					{"GENDER", 0},
					{ "HAIR", 0 },
					{"HAIR_COLOR", 0},
					{"EYE_COLOR", 0},
					{"SKIN_COLOR", 0},
					{"TOP", 0},
					{"TOP_COLOR", 0},
					{"BOTTOM", 0},
					{"BOTTOM_COLOR", 0},
					{"ARM_GEAR", 0},
					{"ARM_GEAR_COLOR", 0},
					{"LEFT_HAND_EQUIPMENT", 0},
					{"RIGHT_HAND_EQUIPMENT", 0},
				}}
			}
		};

		std::ofstream file_out("CharacterInfos.json");
		if (file_out.is_open()) {
			file_out << newSave[0].dump(4); //スペース
			file_out.close();
		}
	}

	std::ifstream file_in("CharacterInfos.json");

	nlohmann::json savedData;

	file_in >> savedData;

	file_in.close();

	PLAYER_CHARACTER_DATA.SetCharacterInfos(savedData);

	PLAYER_CHARACTER_DATA.ParseData();
}

/**************************************************************************//**
	@brief		キャラクリに移動
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4CState::InitState::Execute(float elapsedTime)
{
	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
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
	CameraManager::Instance().GetCamera()->SetLookAt(
		SceneCharacter_E4CState::cameraSettings.back().at(0), SceneCharacter_E4CState::cameraSettings.back().at(1), SceneCharacter_E4CState::cameraSettings.back().at(2)
	);

	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	SceneCharacter_E4CState::btnCharacterLeft = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		setCurrentStateLeft();
		});
	SceneCharacter_E4CState::btnCharacterLeft->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.1f });
	SceneCharacter_E4CState::btnCharacterLeft->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	SceneCharacter_E4CState::btnCharacterLeft->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(SceneCharacter_E4CState::btnCharacterLeft);

	SceneCharacter_E4CState::btnCharacterCenter = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		setCurrentStateCenter();
		});
	SceneCharacter_E4CState::btnCharacterCenter->SetPosition({ SCREEN_W * 0.375f, SCREEN_H * 0.1f });
	SceneCharacter_E4CState::btnCharacterCenter->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	SceneCharacter_E4CState::btnCharacterCenter->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(SceneCharacter_E4CState::btnCharacterCenter);

	SceneCharacter_E4CState::btnCharacterRight = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		setCurrentStateRight();
		});
	SceneCharacter_E4CState::btnCharacterRight->SetPosition({ SCREEN_W * 0.7f, SCREEN_H * 0.1f });
	SceneCharacter_E4CState::btnCharacterRight->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	SceneCharacter_E4CState::btnCharacterRight->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(SceneCharacter_E4CState::btnCharacterRight);

	for (size_t i = 0; i < PLAYER_CHARACTER_DATA.GetCharacterInfosData().size(); i++)
	{
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].save != "")
		{
			PlayerCharacterData::CharacterInfo charInfo =
			{
				true,																// visible
				PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].save,				// save
				{																	//Character
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::GENDER],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::HAIR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::HAIR_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::EYE_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::SKIN_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::TOP],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::TOP_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::BOTTOM],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::BOTTOM_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::ARM_GEAR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::ARM_GEAR_COLOR],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT],
					PLAYER_CHARACTER_DATA.GetCharacterInfosData()[i].Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT],
				}
			};

			owner->m_previewCharacters.at(i) = std::make_unique<NonPlayerCharacter>(charInfo);
			owner->m_previewCharacters.at(i)->SetPosition({ 3.5f - 3.5f * i, 0.0f, 5.0f });
			owner->m_previewCharacters.at(i)->SetKinematic(true);
		}
	}

	for (auto& it : owner->m_previewCharacters)
	{
		if (it != nullptr)
			it->SetMenuVisibility(true);
	}
}
/**************************************************************************//**
	@brief		実行
	@param[in]    elapsedTime
*//***************************************************************************/

void SceneCharacter_E4CState::CharacterSelectionState::Execute(float elapsedTime)
{
}
/**************************************************************************//**
	@brief	終わり
*//***************************************************************************/

void SceneCharacter_E4CState::CharacterSelectionState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

/**************************************************************************//**
	@brief	左キャラを選択
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterSelectionState::setCurrentStateLeft()
{
	SceneCharacter_E4CState::currentState = 0;
	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
}

/**************************************************************************//**
	@brief	中キャラを選択
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterSelectionState::setCurrentStateCenter()
{
	SceneCharacter_E4CState::currentState = 1;
	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
}

/**************************************************************************//**
	@brief	右キャラを選択
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterSelectionState::setCurrentStateRight()
{
	SceneCharacter_E4CState::currentState = 2;
	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
}

/**************************************************************************//**
	@brief	キャラクリ
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Enter()
{
	CameraManager::Instance().GetCamera()->SetLookAt(
		cameraSettings.at(SceneCharacter_E4CState::currentState).at(0),
		cameraSettings.at(SceneCharacter_E4CState::currentState).at(1),
		cameraSettings.at(SceneCharacter_E4CState::currentState).at(2)
	);

	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	auto CurrentData = PLAYER_CHARACTER_DATA.GetCharacterInfosData();

	if (CurrentData.at(SceneCharacter_E4CState::currentState).save == "")
	{
		PlayerCharacterData::CharacterInfo charInfo = {
		true,									// visible
		saveNames.at(SceneCharacter_E4CState::currentState),				// save
		{										//Character
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
		};

		CurrentData.at(SceneCharacter_E4CState::currentState) = charInfo;

		owner->m_previewCharacters.at(SceneCharacter_E4CState::currentState) = std::make_unique<NonPlayerCharacter>(charInfo);

		owner->m_previewCharacters.at(SceneCharacter_E4CState::currentState)->SetPosition(characterPositions.at(SceneCharacter_E4CState::currentState));
		owner->m_previewCharacters.at(SceneCharacter_E4CState::currentState)->SetKinematic(true);
	}
	else
	{
		SceneCharacter_E4CState::previousCharacterInfo = PLAYER_CHARACTER_DATA.GetCharacterInfosData().at(SceneCharacter_E4CState::currentState);
		SceneCharacter_E4CState::currentCharacterInfo = PLAYER_CHARACTER_DATA.GetCharacterInfosData().at(SceneCharacter_E4CState::currentState);
	}

	PLAYER_CHARACTER_DATA.SetCharacterInfosData(CurrentData);

	for (size_t i = 0; i < owner->m_previewCharacters.size(); i++)
	{
		if (owner->m_previewCharacters.at(i) != nullptr)
		{
			if (i != SceneCharacter_E4CState::currentState)
			{
				owner->m_previewCharacters.at(i)->SetMenuVisibility(false);
			}
		}
	}

	SceneCharacter_E4CState::btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/back.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
		});
	SceneCharacter_E4CState::btnStartCharacter->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.05f });
	SceneCharacter_E4CState::btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(SceneCharacter_E4CState::btnStartCharacter);

	SceneCharacter_E4CState::btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	SceneCharacter_E4CState::btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	SceneCharacter_E4CState::btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(SceneCharacter_E4CState::btnStartCharacter);

	WidgetUpDown* GenderSelector = new WidgetUpDown("SHIELD", &SceneCharacter_E4CState::currentCharacterInfo.Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT], 0, 2);
	WidgetUpDown* WeaponSelector = new WidgetUpDown("WEAPON", &SceneCharacter_E4CState::currentCharacterInfo.Character.pattern[PlayerCharacterData::APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT], 0, 1);

	SceneCharacter_E4CState::background = RESOURCE.LoadSpriteResource("Data/Sprites/UI/keybinds/key_Left.png");
	DirectX::XMFLOAT2 size = SceneCharacter_E4CState::background->GetTextureSize() * 0.2f;

	DirectX::XMFLOAT2 position = {
		(SCREEN_W - size.x) / 1.2f,
		(SCREEN_H - size.y) / 2.0f
	};
	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	float optionHeight = size.y / 5.0f;
	DirectX::XMFLOAT2 optionPos = { position.x + 20.0f, 20.0f };
	DirectX::XMFLOAT2 optionPos2 = { position.x + 20.0f, 120.0f };

	GenderSelector->SetSize({ size.x - 40.0f , optionHeight });
	WeaponSelector->SetSize({ size.x - 40.0f , optionHeight });

	GenderSelector->SetPosition(optionPos);
	WeaponSelector->SetPosition(optionPos2);

	UIManager::Instance().Register(GenderSelector);
	UIManager::Instance().Register(WeaponSelector);
}

/**************************************************************************//**
	@brief		キャラ設定を変わる
	@param[in]    elapsedTime
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Execute(float elapsedTime)
{
	for (size_t i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
	{
		if (SceneCharacter_E4CState::previousCharacterInfo.Character.pattern[i] != SceneCharacter_E4CState::currentCharacterInfo.Character.pattern[i])
		{
			owner->m_previewCharacters.at(SceneCharacter_E4CState::currentState)->LoadAppearance(SceneCharacter_E4CState::currentCharacterInfo.Character.pattern);
			SceneCharacter_E4CState::previousCharacterInfo.Character = SceneCharacter_E4CState::currentCharacterInfo.Character;
		}
	}
}

/**************************************************************************//**
	@brief	終わり、データーをPLAYER_CHARACTER_DATAとJSONにセーブ
*//***************************************************************************/
void SceneCharacter_E4CState::CharacterCreationState::Exit()
{
	auto CurrentData = PLAYER_CHARACTER_DATA.GetCharacterInfosData();

	CurrentData.at(SceneCharacter_E4CState::currentState).Character = SceneCharacter_E4CState::currentCharacterInfo.Character;

	PLAYER_CHARACTER_DATA.SetCharacterInfosData(CurrentData);

	PLAYER_CHARACTER_DATA.SaveData();

	PLAYER_CHARACTER_DATA.SetCurrentCharacter(SceneCharacter_E4CState::currentState);

	UI.Clear();
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