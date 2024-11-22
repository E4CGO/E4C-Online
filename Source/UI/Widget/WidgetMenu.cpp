#include "WidgetMenu.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "UI/Widget/WidgetButtonText.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"

WidgetMenu::WidgetMenu()
{
	background = RESOURCE.LoadSpriteResource("Data/Sprites/big_background.png");
	m_size = background->GetTextureSize();
	// 高さを編集
	float rate = SCREEN_H / m_size.y; // 拡大縮小率
	m_size = m_size * rate;

	m_position.x = -m_size.x; // 左に隠す

	options.push_back(new WidgetButtonText("オプション", [&](Widget*) {
		if (this->settingWindow == nullptr) this->settingWindow = new WidgetSettingWindow;
		}));

	options.push_back(new WidgetButtonText("タイトル画面へ", [&](Widget*) {
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
		}));
}
WidgetMenu::~WidgetMenu()
{
	for (WidgetButton*& option : options)
	{
		delete option;
	}
	options.clear();
	if (settingWindow) delete settingWindow;
}

void WidgetMenu::Show()
{
	enable = true;
	CURSOR_ON;
	TPSCamera.SetEnable(false);
}
void WidgetMenu::Hide()
{
	enable = false;
	CURSOR_OFF;
	TPSCamera.SetEnable(true);
}

DirectX::XMFLOAT2 WidgetMenu::GetOptionsSize()
{
	DirectX::XMFLOAT2 optionSize = {};
	for (WidgetButton*& option : options)
	{
		optionSize = option->GetSize();
	}
	return optionSize;
}

void WidgetMenu::Update(float elapsedTime)
{
	if (enable && m_position.x < 0) // 左から出現
	{
		m_position.x += elapsedTime * (m_size.x / moveSpeed); // 1秒で出現
		if (m_position.x > 0) m_position.x = 0.0f;
	}
	if (!enable && m_position.x > -m_size.x) // 左に消える
	{
		m_position.x -= elapsedTime * (m_size.x / moveSpeed); // 1秒で消える
		if (m_position.x < -m_size.x) m_position.x = -m_size.x;
	}

	DirectX::XMFLOAT2 optionSize = GetOptionsSize();
	DirectX::XMFLOAT2 optionPosition = m_position + ((m_size - optionSize) * 0.5f);
	for (WidgetButton*& option : options)
	{
		option->SetPosition(optionPosition);
		option->Update(elapsedTime);
		optionPosition.y += option->GetSize().y + 10.0f; // padding 10
	}
	if (settingWindow)
	{
		settingWindow->Update(elapsedTime);
		if (settingWindow->IsEnd())
		{
			delete settingWindow;
			settingWindow = nullptr;
		}
	}
}
void WidgetMenu::Render(const RenderContext& rc)
{
	T_TEXT.End();
	T_TEXT.Begin();
	if (m_position.x > -m_size.x)
	{
		background->Render(
			rc.deviceContext,
			m_position.x, m_position.y, 0,
			m_size.x, m_size.y,
			20.0f, 20.0f,
			2008, 3012,
			0.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		);

		for (size_t i = 0; i < options.size(); i++)
		{
			options.at(i)->Render(rc);
		}
	}
	if (settingWindow) settingWindow->Render(rc);
}

void WidgetMenu::RenderDX12(const RenderContextDX12& rc)
{
}