#include "WidgetGem.h"

#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#include <string>
#include <shellapi.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Network/url.h"
#include "TAKOEngine/Tool/Encode.h"

#include "GameData.h"

WidgetGem::WidgetGem() : WidgetButtonImage("", "Data/Sprites/barmid_ready.png", [&](Widget*)
	{
		this->xhr = new HttpRequest(URL(Url::USER));
		this->xhr->SetToken(GAME_DATA.GetSetting().token);
		this->xhr->Send();

		this->enable = false;
	})
{
	chargeBtn = new WidgetButtonText("チャージ", [&](Widget*) {
		ShellExecute(0, 0, Encode::string_to_wstring(URL(Url::CHARGE_PAGE)).c_str(), 0, 0, SW_SHOW);
		});
	m_size = { 300.0f, 150.0f };
	gem = RESOURCE.LoadSpriteResource("Data/Sprites/gem.png");
}

void WidgetGem::Update(float elapsedTime)
{
	WidgetButtonImage::Update(elapsedTime);

	chargeBtn->SetPosition({ m_position.x, m_position.y - chargeBtn->GetSize().y });
	chargeBtn->Update(elapsedTime);
	if (xhr != nullptr)
	{
		if (xhr->IsDone())
		{
			HttpResponse response = xhr->GetResponse();
			if (response.status == 200)
			{
				GAME_DATA.GetUser().gems = response.data["gems"];
			}
			delete xhr;
			xhr = nullptr;
			enable = true;
		}
	}
}

void WidgetGem::Render(const RenderContext& rc)
{
	WidgetButtonImage::Render(rc);
	chargeBtn->Render(rc);

	// 読み込み完了
	if (xhr == nullptr)
	{
		gem->Render(
			rc.deviceContext,
			m_position.x * 1.1f, m_position.y + m_size.y * 0.05f, 0,
			m_size.y * 0.9f, m_size.y * 0.9f
		);

		std::string count = " x ";
		count += std::to_string(GAME_DATA.GetUser().gems);
		T_TEXT.Render(
			FONT_ID::MsGothic,
			count.c_str(),
			m_position.x * 1.1f + m_size.y * 0.8f, m_position.y + m_size.y / 2.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			0,
			FONT_ALIGN::LEFT,
			1.0f,
			1
		);
	}
}

void WidgetGem::RenderDX12(const RenderContextDX12& rc)
{
}