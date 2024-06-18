#include "SceneGameState.h"

#include "profiler.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player\PlayerManager.h"

#include "UI/Widget/WidgetCrosshair.h"
#include "UI/Widget/WidgetPlayerHP.h"
#include "UI/Widget/WidgetEnemiesHp.h"
#include "UI/Widget/WidgetSkillTimer.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"

#include "GameData.h"

// 接続待ちステート
void SceneGameState::WaitingState::Enter()
{
	connecting = new WidgetText("接続待ち……");
	connecting->SetPosition({ 1.0f, SCREEN_H - 1.0f });
	connecting->SetScale(0.7f);
	connecting->SetBorder(1);
	connecting->SetAlign(FONT_ALIGN::BOTTOM_LEFT);
	UI.Register(connecting);

	connectionCount = new WidgetText;
	connectionCount->SetPosition({ SCREEN_W - 1.0f, 1.0f });
	connectionCount->SetScale(1.0f);
	connecting->SetBorder(1);
	connectionCount->SetAlign(FONT_ALIGN::TOP_RIGHT);
	UI.Register(connectionCount);

	description = new WidgetText("↑↓：キャラクター選択　←→：色選択　スペース：準備完了\n全員準備完了すればゲーム開始");
	description->SetPosition({ SCREEN_W * 0.5f, 200.0f });
	description->SetBorder(1);
	description->SetAlign(FONT_ALIGN::TOP);
	UI.Register(description);

	charaDescription = new WidgetText("");
	charaDescription->SetPosition({ SCREEN_W * 0.5f, SCREEN_H - 300.0f });
	charaDescription->SetBorder(1);
	charaDescription->SetAlign(FONT_ALIGN::TOP);
	UI.Register(charaDescription);
}
void SceneGameState::WaitingState::Execute(float elapsedTime)
{
	owner->UpdateConnection();

	std::string text = std::to_string(owner->GetNetworkController()->GetClientCount());
	text += "/" + std::to_string(MAX_CONNECTION);
	connectionCount->SetText(text.c_str());

	// キャラ紹介
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	if (player)
	{
		std::string d = "方向キー：移動　スペース：ジャンプ　左CTRL：回避";
		switch (player->GetClassType())
		{
		case PLAYER_CLASS::BARBARIAN:
			d = "バーバリアン：\n左クリック：一般攻撃　　右クリック：斧投げ\n" + d;
			break;
		case PLAYER_CLASS::KNIGHT:
			d = "ナイト：\n左クリック：一般攻撃　　右クリック：ブロック\n" + d;
			break;
		case PLAYER_CLASS::ROGUE:
			d = "レンジャー：\n左クリック：一般ショート　　右クリック：エイム連射\n" + d;
			break;
		}
		charaDescription->SetText(d.c_str());
	}
}
void SceneGameState::WaitingState::Exit()
{
	UI.Remove(connecting);
	UI.Remove(connectionCount);
	UI.Remove(description);
	UI.Remove(charaDescription);
}

// 準備完了ステート（カウントダウン）
void SceneGameState::ReadyState::Enter()
{
	// カウントダウンタイマー
	startTimer = START_TIME;

	// UI表示
	countDownText = new WidgetText;
	countDownText->SetPosition({ SCREEN_W / 2.0f, SCREEN_H / 2.0f });
	countDownText->SetAlign(FONT_ALIGN::CENTER);
	countDownText->SetColor({ 1, 1, 1, 1 });
	countDownText->SetBorder(3);

	UI.Register(countDownText);
}
void SceneGameState::ReadyState::Execute(float elapsedTime)
{
	startTimer -= elapsedTime;
	if (startTimer <= 0) {
		startTimer = 0;
		owner->GetNetworkController()->GameStart();
	}
	owner->UpdateConnection();

	countDownText->SetText(std::to_string(static_cast<int>(ceil(startTimer))).c_str());
}
void SceneGameState::ReadyState::Exit()
{
	UI.Remove(countDownText);
}

// ゲームステート
void SceneGameState::GameState::Enter()
{
	Player* player = PLAYERS.GetPlayerById(GAME_DATA.GetClientId());
	TPSCamera.SetPlayer(player);
	TPSCamera.SetEnable(true);
	player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));

	CURSOR_OFF;

	UI.Register(new WidgetCrosshair);
	UI.Register(new WidgetEnemiesHp);
	UI.Register(new WidgetPlayerHP);
	UI.Register(new WidgetSkillTimer);
}
void SceneGameState::GameState::Execute(float elapsedTime)
{
	owner->UpdateGame(elapsedTime);
}
void SceneGameState::GameState::Exit()
{
}

// ゲームオーバーステート
void SceneGameState::GameOverState::Enter()
{
	UI.Clear();
	WidgetText* text = new WidgetText("GAME OVER");
	text->SetPosition({ SCREEN_W * 0.5f, SCREEN_H * 0.5f });
	text->SetScale(2.0f);
	text->SetAlign(FONT_ALIGN::CENTER);
	text->SetBorder(2);
	text->SetBorderColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	UI.Register(text);
	timer = 3.0f;
}
void SceneGameState::GameOverState::Execute(float elapsedTime)
{
	timer -= elapsedTime;

	if (timer < 0.0f) SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
}
void SceneGameState::GameOverState::Exit()
{
}

// ゲームオーバーステート
void SceneGameState::WinState::Enter()
{
	UI.Clear();
	WidgetText* text = new WidgetText("VICTORY");
	text->SetPosition({ SCREEN_W * 0.5f, SCREEN_H * 0.5f });
	text->SetScale(2.0f);
	text->SetAlign(FONT_ALIGN::CENTER);
	text->SetBorder(2);
	text->SetBorderColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	UI.Register(text);
	timer = 3.0f;
}
void SceneGameState::WinState::Execute(float elapsedTime)
{
	timer -= elapsedTime;

	if (timer < 0.0f) SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
}
void SceneGameState::WinState::Exit()
{
}