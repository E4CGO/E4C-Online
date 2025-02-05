#include "WidgetPauseMenu.h"

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "Scene/SceneManager.h"
#include "Scene/GameLoop/SceneTitle/SceneTitle_E4C.h"


/**************************************************************************//**
	@brief	UIの設定
*//***************************************************************************/
WidgetPauseMenu::WidgetPauseMenu()
{
	if (T_GRAPHICS.isDX11Active)
	{
		background = RESOURCE.LoadSpriteResource("Data/Sprites/UI/Game/frame.png");
		m_size = background->GetTextureSize() * 1.0f;
		m_position = {
			(SCREEN_W - m_size.x) / 2.0f,
			(SCREEN_H - m_size.y) / 2.0f
		};
	}
	else
	{
		backgroundDX12 = RESOURCE.LoadSpriteResourceDX12("Data/Sprites/UI/Game/frame.png");
		m_size = backgroundDX12->GetTextureSize() * 1.0f;
		m_position = {
			(SCREEN_W - m_size.x) / 2.0f,
			(SCREEN_H - m_size.y) / 2.0f
		};
	}

	// 閉じ
	m_pauseButton = new WidgetButtonImage("", "Data/Sprites/UI/Game/settings_d.png", "Data/Sprites/UI/Game/settings_h.png", [&](Widget*)
		{
			this->Trigger();
		});
	m_pauseButton->SetSize({ m_pauseButton->GetSize().x * 0.2f * SCREEN_W / 1920.0f, m_pauseButton->GetSize().y * 0.2f * SCREEN_H / 1080.0f });
	m_pauseButton->SetPosition({ SCREEN_W * 0.95f, SCREEN_H * 0.02f });

	m_resumeButton = new WidgetButtonImage("", "Data/Sprites/UI/Game/RESUME_d.png", "Data/Sprites/UI/Game/RESUME_h.png", [&](WidgetButton*) {
		this->isActive = false;
		});
	m_resumeButton->SetPosition({ SCREEN_W * .5f - m_resumeButton->GetSize().x * SCREEN_W / 1920.0f * 0.5f, SCREEN_H * 0.3f });
	m_resumeButton->SetSize({ m_resumeButton->GetSize().x * SCREEN_W / 1920.0f, m_resumeButton->GetSize().y * SCREEN_H / 1080.0f });

	m_toTitleButton = new WidgetButtonImage("", "Data/Sprites/UI/Game/MAIN MENU_d.png", "Data/Sprites/UI/Game/MAIN MENU_h.png", [&](WidgetButton*) {
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle_E4C));
		});
	m_toTitleButton->SetPosition({ SCREEN_W * .5f - m_resumeButton->GetSize().x * SCREEN_W / 1920.0f * 0.5f, SCREEN_H * 0.3f + m_resumeButton->GetSize().y * 1.5f * 1.0f });
	m_toTitleButton->SetSize({ m_toTitleButton->GetSize().x * SCREEN_W / 1920.0f, m_toTitleButton->GetSize().y * SCREEN_H / 1080.0f });

	m_settingsButton = new WidgetButtonImage("", "Data/Sprites/UI/Game/OPTION_d.png", "Data/Sprites/UI/Game/OPTION_h.png", [&](WidgetButton*) {
		m_settingsWindow = new WidgetSettingWindow;
		this->isSettingsActive = true;
		});
	m_settingsButton->SetPosition({ SCREEN_W * .5f - m_resumeButton->GetSize().x * SCREEN_W / 1920.0f * 0.5f, SCREEN_H * 0.3f + m_resumeButton->GetSize().y * 1.5f * 2.0f });
	m_settingsButton->SetSize({ m_settingsButton->GetSize().x * SCREEN_W / 1920.0f,  m_settingsButton->GetSize().y * SCREEN_H / 1080.0f });

	m_exitButton = new WidgetButtonImage("", "Data/Sprites/UI/Game/EXIT_d.png", "Data/Sprites/UI/Game/EXIT_h.png", [&](WidgetButton*) {
		PostMessage(TentacleLib::hWnd, WM_QUIT, 0, 0);
		});
	m_exitButton->SetPosition({ SCREEN_W * .5f - m_resumeButton->GetSize().x * SCREEN_W / 1920.0f * 0.5f, SCREEN_H * 0.3f + m_resumeButton->GetSize().y * 1.5f * 3.0f });
	m_exitButton->SetSize({ m_exitButton->GetSize().x * SCREEN_W / 1920.0f, m_exitButton->GetSize().y * SCREEN_H / 1080.0f });
}

/**************************************************************************//**
	@brief	UIをアップデート
	@param[in]    elapsedTime
*//***************************************************************************/
void WidgetPauseMenu::Update(float elapsedTime)
{
	if (T_INPUT.KeyDown(VK_ESCAPE) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::BACK))
	{
		this->Trigger();
	}

	m_pauseButton->Update(elapsedTime);

	if (isActive)
	{
		m_resumeButton->Update(elapsedTime);
		m_toTitleButton->Update(elapsedTime);
		m_settingsButton->Update(elapsedTime);
		m_exitButton->Update(elapsedTime);

		if (isSettingsActive)
		{
			if (m_settingsWindow->IsEnd())
			{
				isSettingsActive = false;
				delete m_settingsWindow;
				m_settingsWindow = nullptr;
			}
			else
			{
				m_settingsWindow->Update(elapsedTime);
			}
		}
	}
}

/**************************************************************************//**
	@brief	DX11の描画
	@param[in]    rc
*//***************************************************************************/
void WidgetPauseMenu::Render(const RenderContext& rc)
{
	if (isActive)
	{
		background->Render(rc.deviceContext, m_position.x, m_position.y, 0, m_size.x, m_size.y);

		m_resumeButton->Render(rc);
		m_toTitleButton->Render(rc);
		m_settingsButton->Render(rc);
		m_exitButton->Render(rc);

		if (isSettingsActive) m_settingsWindow->Render(rc);
	}
	m_pauseButton->Render(rc);
}

/**************************************************************************//**
	@brief	DX12の描画
	@param[in]    rc
*//***************************************************************************/
void WidgetPauseMenu::RenderDX12(const RenderContextDX12& rc)
{
	if (isActive)
	{
		backgroundDX12->Begin(rc);
		backgroundDX12->Draw(
			m_position.x, m_position.y,
			m_size.x, m_size.y,
			0,
			1, 1, 1, 1);
		backgroundDX12->End(rc.d3d_command_list);

		m_resumeButton->RenderDX12(rc);
		m_toTitleButton->RenderDX12(rc);
		m_settingsButton->RenderDX12(rc);
		m_exitButton->RenderDX12(rc);

		if (isSettingsActive) m_settingsWindow->RenderDX12(rc);
	}
	m_pauseButton->RenderDX12(rc);
}