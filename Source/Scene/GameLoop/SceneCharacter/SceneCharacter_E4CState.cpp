#include "SceneCharacter_E4CState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

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
			file_out << newSave[0].dump(4); //スペース
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
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATIONLEFT);
		});
	btnCharacterLeft->SetPosition({ SCREEN_W * 0.1f, SCREEN_H * 0.1f });
	btnCharacterLeft->SetSize({ SCREEN_W * 0.25f, SCREEN_H * 0.8f });
	btnCharacterLeft->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 0.5f });
	UIManager::Instance().Register(btnCharacterLeft);

	btnCharacterCenter = new WidgetButtonImage("", "Data/Sprites/big_background.t.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::CHARACTERCREATIONCENTER);
		});
	btnCharacterCenter->SetPosition({ SCREEN_W * 0.4f, SCREEN_H * 0.1f });
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
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 6.0f, 2.0f, 9.0f },			// 視点
		{ -3.0f, 0.0f, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	owner->m_previewCharacters[1]->SetMenuVisibility(false);
	owner->m_previewCharacters[2]->SetMenuVisibility(false);
}
void SceneCharacter_E4CState::CharacterCreationStateLeft::Execute(float elapsedTime)
{

}
void SceneCharacter_E4CState::CharacterCreationStateLeft::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::CharacterCreationStateCenter::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	CameraManager& cameraManager = CameraManager::Instance();
	
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 6.0, 2.0f, 9.0f },			// 視点
		{ -3.0f, 0.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	owner->m_previewCharacters[0]->SetMenuVisibility(false);
	owner->m_previewCharacters[2]->SetMenuVisibility(false);
}
void SceneCharacter_E4CState::CharacterCreationStateCenter::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationStateCenter::Exit()
{
	UI.Clear();
	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// タイトルステート
void SceneCharacter_E4CState::CharacterCreationStateRight::Enter()
{
	btnStartCharacter = new WidgetButtonImage("", "Data/Sprites/UI/start.png", [&](WidgetButton*) {
		owner->GetStateMachine()->ChangeState(SceneCharacter_E4C::STATE::START);
		});
	btnStartCharacter->SetPosition({ SCREEN_W * 0.5f - 163.0f * 0.5f * 1.5f, SCREEN_H * 0.8f });
	btnStartCharacter->SetSize({ 196.0f * 1.5f, 92.0f * 1.5f });
	UIManager::Instance().Register(btnStartCharacter);

	CameraManager& cameraManager = CameraManager::Instance();
	
	CameraManager::Instance().GetCamera()->SetLookAt(
		{ 6.0, 2.0f, 9.0f },			// 視点
		{ -3.0f, 0.0, 0.0f },					// 注視点
		{ 0.036f, 0.999f, -0.035f }				// 上ベクトル
	);

	owner->cameraController->SyncCameraToController(CameraManager::Instance().GetCamera());

	owner->m_previewCharacters[0]->SetMenuVisibility(false);
	owner->m_previewCharacters[1]->SetMenuVisibility(false);
}
void SceneCharacter_E4CState::CharacterCreationStateRight::Execute(float elapsedTime)
{
}
void SceneCharacter_E4CState::CharacterCreationStateRight::Exit()
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