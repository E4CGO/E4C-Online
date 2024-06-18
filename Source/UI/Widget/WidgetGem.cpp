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
	chargeBtn = new WidgetButtonText("ƒ`ƒƒ[ƒW", [&](Widget*) {
		ShellExecute(0, 0, Encode::char_to_wchar(URL(Url::CHARGE_PAGE)), 0, 0, SW_SHOW);
		});
	size = { 300.0f, 150.0f };
	gem = RESOURCE.LoadSpriteResource("Data/Sprites/gem.png");
}

void WidgetGem::Update(float elapsedTime)
{
	WidgetButtonImage::Update(elapsedTime);

	chargeBtn->SetPosition({ position.x, position.y - chargeBtn->GetSize().y });
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

	// “Ç‚Ýž‚ÝŠ®—¹
	if (xhr == nullptr)
	{
		gem->Render(
			rc.deviceContext,
			position.x * 1.1f, position.y + size.y * 0.05f, 0,
			size.y * 0.9f, size.y * 0.9f
		);

		std::string count = " x ";
		count += std::to_string(GAME_DATA.GetUser().gems);
		T_TEXT.Render(
			FONT_ID::MsGothic,
			count.c_str(),
			position.x * 1.1f + size.y * 0.8f, position.y + size.y / 2.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			0,
			FONT_ALIGN::LEFT,
			1.0f,
			1
		);
	}
}