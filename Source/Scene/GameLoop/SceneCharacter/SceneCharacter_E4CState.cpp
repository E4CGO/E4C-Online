#include "SceneCharacter_E4CState.h"

#include <fstream>

#include "TAKOEngine/GUI/UIManager.h"
#include "PlayerCharacterData.h"

#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

WidgetButtonImage* btnCharacterLeft = nullptr;
WidgetButtonImage* btnCharacterCenter = nullptr;
WidgetButtonImage* btnCharacterRight = nullptr;

WidgetButtonImage* btnStartCharacter = nullptr;

inline bool FileExist(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

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

void SceneCharacter_E4CState::InitState::Execute(float elapsedTime)
{
	owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
}

void SceneCharacter_E4CState::InitState::Exit()
{
}

// タイトルステート
void SceneCharacter_E4CState::CharacterSelectionState::Enter()
{
	Camera& camera = Camera::Instance();

	camera.SetLookAt(
		{ 0, 3.35f, 12.4f },				// 視点
		{ 0.06f, 0.650, 0.730f },					// 注視点
		{ 0.0f, 0.1f, 0.0f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(camera);

	btnCharacterLeft = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATIONLEFT);
		});
	btnCharacterLeft->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.1f });
	btnCharacterLeft->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	btnCharacterLeft->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterLeft);

	btnCharacterCenter = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATIONCENTER);
		});
	btnCharacterCenter->SetPosition({ SCREEN_W * 0.375f, SCREEN_H * 0.1f });
	btnCharacterCenter->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	btnCharacterCenter->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterCenter);

	btnCharacterRight = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATIONRIGHT);
		});
	btnCharacterRight->SetPosition({ SCREEN_W * 0.7f, SCREEN_H * 0.1f });
	btnCharacterRight->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	btnCharacterRight->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterRight);

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
void SceneCharacter_E4CState::CharacterSelectionState::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterSelectionState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::CharacterCreationStateLeft::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/back.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.05f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	if (owner->m_previewCharacters.at(1) != nullptr) owner->m_previewCharacters.at(1)->SetMenuVisibility(false);
	if (owner->m_previewCharacters.at(2) != nullptr) owner->m_previewCharacters.at(2)->SetMenuVisibility(false);

	Camera& camera = Camera::Instance();

	camera.SetLookAt(
		{ 3.7, 2.1f, 10.5f },				// 視点
		{ 3.5f, -2.3, -15.0f },					// 注視点
		{ 0.0f, 1.0f, 0.0f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(camera);

	auto CurrentData = PLAYER_CHARACTER_DATA.GetCharacterInfosData();

	if (CurrentData.at(0).save == "")
	{
		PlayerCharacterData::CharacterInfo charInfo = {
		true,			// visible
		"Left",				// save
		{				//Character
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
		};

		CurrentData.at(0) = charInfo;

		owner->m_previewCharacters.at(0) = std::make_unique<NonPlayerCharacter>(charInfo);
		owner->m_previewCharacters.at(0)->SetPosition({ 3.5f, 0.0f, 5.0f });
		owner->m_previewCharacters.at(0)->SetKinematic(true);
	}

	PLAYER_CHARACTER_DATA.SetCharacterInfosData(CurrentData);
}
void SceneCharacter_E4CState::CharacterCreationStateLeft::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationStateLeft::Exit()
{
	PLAYER_CHARACTER_DATA.SaveData();

	PLAYER_CHARACTER_DATA.m_CurrentSaveState = 0;

	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::CharacterCreationStateCenter::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/back.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.05f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	if (owner->m_previewCharacters.at(1) != nullptr) owner->m_previewCharacters.at(1)->SetMenuVisibility(false);
	if (owner->m_previewCharacters.at(2) != nullptr) owner->m_previewCharacters.at(2)->SetMenuVisibility(false);

	Camera& camera = Camera::Instance();

	camera.SetLookAt(
		{ 0.0, 2.1f, 10.5f },				// 視点
		{ 0.0f, 0.0f, 0.0f },					// 注視点
		{ 0.0f, 1.0f, 0.0f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(camera);

	auto CurrentData = PLAYER_CHARACTER_DATA.GetCharacterInfosData();

	if (CurrentData.at(1).save == "")
	{
		PlayerCharacterData::CharacterInfo charInfo = {
		true,			// visible
		"Center",		// save
		{				//Character
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
		};

		CurrentData.at(1) = charInfo;

		owner->m_previewCharacters.at(1) = std::make_unique<NonPlayerCharacter>(charInfo);
		owner->m_previewCharacters.at(1)->SetPosition({ 0.0f, 0.0f, 5.0f });
		owner->m_previewCharacters.at(1)->SetKinematic(true);
	}

	PLAYER_CHARACTER_DATA.SetCharacterInfosData(CurrentData);
}
void SceneCharacter_E4CState::CharacterCreationStateCenter::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationStateCenter::Exit()
{
	PLAYER_CHARACTER_DATA.SaveData();

	PLAYER_CHARACTER_DATA.m_CurrentSaveState = 1;

	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::CharacterCreationStateRight::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/back.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERSELECTION);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.05f, SCREEN_H * 0.05f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	Camera& camera = Camera::Instance();

	camera.SetLookAt(
		{ -3.7, 2.1f, 10.5f },				// 視点
		{ -3.5f, -2.3, -15.0f },					// 注視点
		{ 0.0f, 1.0f, 0.0f }				// 上ベクトル
	);

	if (owner->m_previewCharacters.at(0) != nullptr) owner->m_previewCharacters.at(1)->SetMenuVisibility(false);
	if (owner->m_previewCharacters.at(1) != nullptr) owner->m_previewCharacters.at(0)->SetMenuVisibility(false);

	owner->cameraController->SyncCameraToController(camera);

	auto CurrentData = PLAYER_CHARACTER_DATA.GetCharacterInfosData();

	if (CurrentData.at(2).save == "")
	{
		PlayerCharacterData::CharacterInfo charInfo = {
		true,			// visible
		"Right",		// save
		{				//Character
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
		};

		CurrentData.at(2) = charInfo;

		owner->m_previewCharacters.at(2) = std::make_unique<NonPlayerCharacter>(charInfo);
		owner->m_previewCharacters.at(2)->SetPosition({ -3.5f, 0.0f, 5.0f });
		owner->m_previewCharacters.at(2)->SetKinematic(true);
	}

	PLAYER_CHARACTER_DATA.SetCharacterInfosData(CurrentData);
}
void SceneCharacter_E4CState::CharacterCreationStateRight::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationStateRight::Exit()
{
	PLAYER_CHARACTER_DATA.SaveData();

	PLAYER_CHARACTER_DATA.m_CurrentSaveState = 2;

	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::StartState::Enter()
{
	PLAYER_CHARACTER_DATA.SaveData();
}
void SceneCharacter_E4CState::StartState::Execute(float elapsedTime)
{
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame_E4C));
}
void SceneCharacter_E4CState::StartState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}