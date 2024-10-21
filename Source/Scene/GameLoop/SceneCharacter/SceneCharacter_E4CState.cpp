#include "SceneCharacter_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "PlayerCharacterData.h"

#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

#include "fstream"

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
		nlohmann::json newSave = PLAYER_CHARACTER_DATA.GetCharacterInfos();

		newSave[0]["Characters"] = {
			{
				{"Save", "CharacterLeft"},
				{"Visible", true},
				{"Character", {
					{"GenderType", 0},
					{ "HeadType", 0 },
					{"BodyType", 0},
					{"WeaponType", 0},
				}}
			},
			{
				{"Save", "CharacterCenter"},
				{"Visible", true},
				{"Character", {
					{"GenderType", 0},
					{"HeadType", 0},
					{"BodyType", 0},
					{"WeaponType", 0},
				}}
			},
			{
				{"Save", "CharacterRight"},
				{"Visible", true},
				{"Character", {
					{"GenderType", 0},
					{"HeadType", 0},
					{"BodyType", 0},
					{"WeaponType", 0},
				}}
			}
		};

		std::ofstream file_out("CharacterInfos.json");
		if (file_out.is_open()) {
			file_out << newSave[0].dump(4);
			file_out.close();
		}
	}

	if (FileExist("CharacterInfos.json"))
	{
		std::ifstream file_in("CharacterInfos.json");

		nlohmann::json savedData;

		file_in >> savedData;

		file_in.close();

		PLAYER_CHARACTER_DATA.SetCharacterInfos(savedData);
	}
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
	PLAYER_CHARACTER_DATA.ParseData();

	btnCharacterLeft = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
		});
	btnCharacterLeft->SetPosition({ SCREEN_W * 0.1f, SCREEN_H * 0.1f });
	btnCharacterLeft->SetSize({ SCREEN_W * 0.2f, SCREEN_H * 0.8f });
	btnCharacterLeft->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterLeft);

	btnCharacterCenter = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
		});
	btnCharacterCenter->SetPosition({ SCREEN_W * 0.4f, SCREEN_H * 0.1f });
	btnCharacterCenter->SetSize({ SCREEN_W * 0.2f, SCREEN_H * 0.8f });
	btnCharacterCenter->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterCenter);

	btnCharacterRight = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATION);
		});
	btnCharacterRight->SetPosition({ SCREEN_W * 0.7f, SCREEN_H * 0.1f });
	btnCharacterRight->SetSize({ SCREEN_W * 0.2f, SCREEN_H * 0.8f });
	btnCharacterRight->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterRight);
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
void SceneCharacter_E4CState::CharacterCreationState::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	if (btnCharacterLeft->IsHover())
	{
		PLAYER_CHARACTER_DATA.SetCharacterVisibility(1, 0);
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.headType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterHeadType(0, 1);
			owner->UpdateCurrentModel(0, 0, 1);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[0].Character.bodyType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterBodyType(0, 1);
			owner->UpdateCurrentModel(0, 1, 1);
		}
	}
	if (btnCharacterCenter->IsHover())
	{
		PLAYER_CHARACTER_DATA.SetCharacterVisibility(1, 1);
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.headType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterHeadType(1, 1);
			owner->UpdateCurrentModel(1, 0, 1);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[1].Character.bodyType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterBodyType(1, 1);
			owner->UpdateCurrentModel(1, 1, 1);
		}
	}
	if (btnCharacterRight->IsHover())
	{
		PLAYER_CHARACTER_DATA.SetCharacterVisibility(1, 2);
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.headType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterHeadType(2, 1);
			owner->UpdateCurrentModel(2, 0, 1);
		}
		if (PLAYER_CHARACTER_DATA.GetCharacterInfosData()[2].Character.bodyType == 0)
		{
			PLAYER_CHARACTER_DATA.SetCharacterBodyType(2, 1);
			owner->UpdateCurrentModel(2, 1, 1);
		}
	}
}
void SceneCharacter_E4CState::CharacterCreationState::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationState::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::StartState::Enter()
{
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