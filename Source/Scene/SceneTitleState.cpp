#include "SceneTitleState.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Network/url.h"

#include "UI/Widget/WidgetGem.h"

// タイトルステート
void SceneTitleState::TitleState::Enter()
{
	title = new WidgetText("P2Pダンジョン・アドベンチャー");
	title->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	title->SetScale(2.0f);
	title->SetBorder(1);
	title->SetBorderColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	title->SetAlign(FONT_ALIGN::CENTER);
	title->SetPosition({ SCREEN_W * 0.5f, SCREEN_H * 0.2f });
	UI.Register(title);

	click = new WidgetText("クリックで始まる");
	click->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	click->SetScale(0.5f);
	click->SetBorder(1);
	click->SetAlign(FONT_ALIGN::TOP);
	click->SetPosition({ SCREEN_W * 0.5f, SCREEN_H * 0.8f });
	UI.Register(click);

	mask = new WidgetImage("");
	mask->SetSize({ SCREEN_W, SCREEN_H });
	mask->SetColor({ 1.0f, 1.0f, 1.0f, 0.7f });
	UI.Register(mask);

	SetCursor(::LoadCursor(NULL, IDC_HAND));
}
void SceneTitleState::TitleState::Execute(float elapsedTime)
{
	// フェイドイン
	DirectX::XMFLOAT4 color = mask->GetColor();
	color.w -= elapsedTime;
	if (color.w < 0.0f) color.w = 0.0f;
	mask->SetColor(color);

	if (color.w == 0.0f)
	{
		// フレッシュ
		timer += elapsedTime * 2.0f;
		while (timer >= DirectX::XMConvertToRadians(180)) timer -= DirectX::XMConvertToRadians(180);
		click->SetColor({ 1.0f, 1.0f, 1.0f, sinf(timer) });
		click->SetBorderColor({ 0.0f, 0.0f, 0.0f, sinf(timer) });

		// 左クリック
		if (T_INPUT.KeyDown(VK_LBUTTON)) owner->GetStateMachine()->ChangeState(SceneTitle::STATE::LOGIN_CHECK);
	}
}
void SceneTitleState::TitleState::Exit()
{
	UI.Clear();

	SetCursor(::LoadCursor(NULL, IDC_HAND));
}

// インプットステート
void SceneTitleState::InputState::Enter()
{
	WidgetImage* mask = new WidgetImage("");
	mask->SetSize({ SCREEN_W, SCREEN_H });
	mask->SetColor({ 1.0f, 1.0f, 1.0f, 0.7f });
	UI.Register(mask);

	// 設定ボタン
	WidgetButtonImage* btnSetting = new WidgetButtonImage("", "Data/Sprites/gear.png", [&](WidgetButton*) {
		this->owner->GetStateMachine()->ChangeState(SceneTitle::STATE::SETTING);
		});
	btnSetting->SetPosition({ SCREEN_W - 192.0f, 30.0f });
	btnSetting->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	btnSetting->SetSize({ 128.0f, 128.0f });
	btnSetting->SetHover([btn = btnSetting](WidgetButton*) {
		btn->GetText()->SetText("設定");
		});
	btnSetting->SetUnhover([btn = btnSetting](WidgetButton*) {
		btn->GetText()->SetText("");
		});
	UI.Register(btnSetting);

	if (GAME_DATA.IsOnline()) // オンラインモード
	{
		if (GAME_DATA.GetUser().id > 0)
		{
			// ログアウトボタン
			WidgetButtonImage* btnLogout = new WidgetButtonImage("", "Data/Sprites/Logout.png", [&](WidgetButton*) {
				GAME_DATA.Logout();
				this->owner->GetStateMachine()->ChangeState(SceneTitle::STATE::LOGIN_CHECK);
				});
			btnLogout->SetPosition({ SCREEN_W - 350.0f, 32.0f });
			btnLogout->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			btnLogout->SetSize({ 128.0f, 128.0f });
			btnLogout->SetHover([btn = btnLogout](WidgetButton*) {
				btn->GetText()->SetText("ログアウト");
				});
			btnLogout->SetUnhover([btn = btnLogout](WidgetButton*) {
				btn->GetText()->SetText("");
				});
			UI.Register(btnLogout);

			// ジェム表示
			WidgetGem* gems = new WidgetGem;
			gems->SetPosition({ 50.0f, SCREEN_H - 50.0f - gems->GetSize().y });
			UI.Register(gems);
		}
		else
		{
			// ログインボタン
			WidgetButtonImage* btnLogin = new WidgetButtonImage("", "Data/Sprites/Login.png", [&](WidgetButton*) {
				this->owner->GetStateMachine()->ChangeState(SceneTitle::STATE::LOGIN);
				});
			btnLogin->SetPosition({ SCREEN_W - 350.0f, 32.0f });
			btnLogin->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			btnLogin->SetSize({ 128.0f, 128.0f });
			btnLogin->SetHover([btn = btnLogin](WidgetButton*) {
				btn->GetText()->SetText("ログイン");
				});
			btnLogin->SetUnhover([btn = btnLogin](WidgetButton*) {
				btn->GetText()->SetText("");
				});
			UI.Register(btnLogin);
		}
	}
	std::string description = "新しい部屋（ホスト）：\nポートを開放し、接続を待つ / ソロプレイする\n\n入室（クライアント）：\n既存の部屋に接続する";
	WidgetText* T = new WidgetText(description.c_str(), 1.0f);
	T->SetPosition({ SCREEN_W * 0.5f, 200.0f });
	T->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	T->SetAlign(FONT_ALIGN::TOP);
	UI.Register(T);

	WidgetText* IP = new WidgetText(GAME_DATA.GetIp().c_str(), 1.0f);
	IP->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	UI.Register(IP);

	WidgetRoomOption* roomOption = new WidgetRoomOption;
	UI.Register(roomOption);
}
void SceneTitleState::InputState::Execute(float elapsedTime)
{
	// 右クリック
	if (T_INPUT.KeyDown(VK_RBUTTON) || T_INPUT.KeyDown(VK_ESCAPE))
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::TITLE);
}
void SceneTitleState::InputState::Exit()
{
	UI.Clear();
}

// 設定ステート
void SceneTitleState::SettingState::Enter()
{
	WidgetImage* mask = new WidgetImage("");
	mask->SetSize({ SCREEN_W, SCREEN_H });
	mask->SetColor({ 1.0f, 1.0f, 1.0f, 0.7f });
	UI.Register(mask);

	settingWindow = new WidgetSettingWindow;
	UI.Register(settingWindow);
}
void SceneTitleState::SettingState::Execute(float elapsedTime)
{
	// 右クリック
	if (settingWindow->IsEnd() || T_INPUT.KeyDown(VK_RBUTTON) || T_INPUT.KeyDown(VK_ESCAPE))
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
}
void SceneTitleState::SettingState::Exit()
{
	UI.Clear();
}

// ログインステート
void SceneTitleState::LoginState::Enter()
{
	WidgetImage* mask = new WidgetImage("");
	mask->SetSize({ SCREEN_W, SCREEN_H });
	mask->SetColor({ 1.0f, 1.0f, 1.0f, 0.7f });
	UI.Register(mask);

	widgetLogin = new WidgetLogin;
	UI.Register(widgetLogin);
}
void SceneTitleState::LoginState::Execute(float elapsedTime)
{
	switch (widgetLogin->GetState())
	{
	case WidgetLogin::STATE::FAIL:
	case WidgetLogin::STATE::INPUT:
		// 右クリック
		if (T_INPUT.KeyDown(VK_RBUTTON) || T_INPUT.KeyDown(VK_ESCAPE))
			owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
		break;
	case WidgetLogin::STATE::SUCCESS:
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::LOGIN_CHECK);
		return;
	}
}
void SceneTitleState::LoginState::Exit()
{
	UI.Clear();
}

void SceneTitleState::LoginCheckState::Enter()
{
	if (!GAME_DATA.IsOnline()) // オフラインモード
	{
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
		return;
	}

	if (GAME_DATA.GetUser().id > 0) // ログイン済み
	{
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
		return;
	}

	if (strlen(GAME_SETTING.token) == 0) // ログイントークンなし
	{
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
		return;
	}

	WidgetImage* mask = new WidgetImage("");
	mask->SetSize({ SCREEN_W, SCREEN_H });
	mask->SetColor({ 1.0f, 1.0f, 1.0f, 0.7f });
	UI.Register(mask);

	xhr = new HttpRequest(URL(Url::USER));
	xhr->SetToken(GAME_DATA.GetSetting().token);
	xhr->Send();
}
void SceneTitleState::LoginCheckState::Execute(float elapsedTime)
{
	if (xhr->IsDone())
	{
		HttpResponse response = xhr->GetResponse();
		if (response.status == 200)
		{
			// ログイン成功
			GAME_DATA.GetUser().id = response.data["id"];
			GAME_DATA.GetUser().useranme = response.data["username"];
			GAME_DATA.GetUser().nickname = response.data["name"];
			GAME_DATA.GetUser().gems = response.data["gems"];
		}
		else
		{
			// ログイン失敗トークン削除
			memset(GAME_DATA.GetSetting().token, 0, sizeof(GAME_DATA.GetSetting().token));
			GAME_DATA.SaveGameSetting();
		}
		delete xhr;
		xhr = nullptr;
		owner->GetStateMachine()->ChangeState(SceneTitle::STATE::INPUT);
	}
}
void SceneTitleState::LoginCheckState::Exit()
{
	if (xhr != nullptr)
	{
		delete xhr;
		xhr = nullptr;
	}
	UI.Clear();
}