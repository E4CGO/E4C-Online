//! @file WidgetSettingWindow.cpp
//! @note

#include "UI/Widget/WidgetSettingWindow.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

#include "TAKOEngine/GUI/UIManager.h"

#include "UI/Widget/WidgetInputBool.h"
#include "UI/Widget/WidgetDragFloat.h"

WidgetSettingWindow::WidgetSettingWindow()
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
	closeBtn = new WidgetButtonImage("", "Data/Sprites/UI/Game/cross.png", [&](Widget*)
		{
			this->isEnd = true;
		});
	DirectX::XMFLOAT2 s = closeBtn->GetSize();
	s.x = m_size.x / 12.0f;
	s.y *= (s.x / closeBtn->GetSize().x);
	closeBtn->SetSize(s);
	closeBtn->SetPosition({ m_position.x + m_size.x - s.x - 30.0f, m_position.y + 20.0f });

	// 確認
	saveBtn = new WidgetButtonImage("", "Data/Sprites/UI/Game/save_d.png", "Data/Sprites/UI/Game/save_h.png", [&](Widget*)
		{
			GAME_DATA.UpdateSetting(cacheGameSetting);
			GAME_DATA.SaveGameSetting();
			this->isEnd = true;
		});
	s = saveBtn->GetSize();
	s.x = m_size.x / 2.0f;
	s.y *= (s.x / saveBtn->GetSize().x);
	saveBtn->SetSize(s);
	saveBtn->SetPosition({ m_position.x + (m_size.x - s.x) * 0.5f, m_position.y + m_size.y - s.y - 60.0f });

	cacheGameSetting = GAME_SETTING;
	// オプション。
	//OptionsList.push_back(new WidgetDragFloat("音量", &cacheGameSetting.CameraSensitivity, 0.0f, 1.0f));
	OptionsList.push_back(new WidgetDragFloat("カメラ感度", &cacheGameSetting.CameraSensitivity, 3.0f, 20.0f));
	OptionsList.push_back(new WidgetInputBool("X軸反転", &cacheGameSetting.CamerainvertX));
	OptionsList.push_back(new WidgetInputBool("Y軸反転", &cacheGameSetting.CamerainvertY));
	OptionsList.push_back(new WidgetInputBool("キーボード操作", &cacheGameSetting.KeyboardInput));
	float optionHeight = m_size.y / 15.0f;
	DirectX::XMFLOAT2 optionPos = { m_position.x + 50.0f, closeBtn->GetPosition().y + closeBtn->GetSize().y + 20.0f };
	for (Widget*& option : OptionsList)
	{
		option->SetSize({ m_size.x - 100.0f , optionHeight });
		option->SetPosition(optionPos);
		optionPos.y += option->GetSize().y + 5.0f;
	}
}
WidgetSettingWindow::~WidgetSettingWindow()
{
	delete closeBtn;
	delete saveBtn;
	for (Widget*& option : OptionsList)
	{
		delete option;
	}
	OptionsList.clear();
}

void WidgetSettingWindow::Update(float elapsedTime)
{
	if (T_INPUT.KeyDown(VK_ESCAPE) || T_INPUT.GamePadKeyDown(GAME_PAD_BTN::BACK))
	{
		isEnd = true;
		return;
	}

	for (Widget*& option : OptionsList)
	{
		option->Update(elapsedTime);
	}
	saveBtn->Update(elapsedTime);
	closeBtn->Update(elapsedTime);
}

void WidgetSettingWindow::Render(const RenderContext& rc)
{
	background->Render(rc.deviceContext, m_position.x, m_position.y, 0, m_size.x, m_size.y);

	closeBtn->Render(rc);
	saveBtn->Render(rc);
	for (Widget*& option : OptionsList)
	{
		option->Render(rc);
	}
}

void WidgetSettingWindow::RenderDX12(const RenderContextDX12& rc)
{
	backgroundDX12->Begin(rc);
	backgroundDX12->Draw(
		m_position.x, m_position.y,
		m_size.x, m_size.y,
		0,
		1, 1, 1, 1);
	backgroundDX12->End(rc.d3d_command_list);

	closeBtn->RenderDX12(rc);
	saveBtn->RenderDX12(rc);
	for (Widget*& option : OptionsList)
	{
		option->RenderDX12(rc);
	}
}