#include "Dialog.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ResourceManager.h"

Dialog::Dialog()
{
	rectangle = RESOURCE.LoadSpriteResource("");
	triangle = RESOURCE.LoadSpriteResource("Data/Sprites/trangle.png");

	dialogSize = { T_GRAPHICS.GetScreenWidth() * 0.8f, T_GRAPHICS.GetScreenHeight() * 0.4f };
	dialogPosition = { T_GRAPHICS.GetScreenWidth() * 0.1f, T_GRAPHICS.GetScreenHeight() * 0.1f };
}

void Dialog::Update(float elapsedTime)
{
	if (!done)
	{
		timer += elapsedTime;

		if ((int)(timer / typeSpeed) >= (int)text.length())
		{
			Done();
			done = true;
		}
	}
}
void Dialog::Render()
{
	if (!visible) return;
	if (dialogOwner != nullptr)
	{
		// オーナー座標計算
		DirectX::XMFLOAT3 arrowTarget = dialogOwner->GetScreenPosition();
		arrowTarget.y -= 100.0f;

		// 三角描画
		triangle->Render(
			T_GRAPHICS.GetDeviceContext(),
			arrowTarget.x - 20.0f, dialogPosition.y + dialogSize.y, 0.0f,
			40.0f, arrowTarget.y - dialogPosition.y - dialogSize.y,
			0.0f,
			dialogColor.x, dialogColor.y, dialogColor.z, dialogColor.w
		);
	}
	// 長方形描画
	rectangle->Render(
		T_GRAPHICS.GetDeviceContext(),
		dialogPosition.x, dialogPosition.y, 0.0f,
		dialogSize.x, dialogSize.y,
		0.0f,
		dialogColor.x, dialogColor.y, dialogColor.z, dialogColor.w
	);
	T_TEXT.Render(
		font,
		name.c_str(),
		dialogPosition.x + dialogPadding.x, dialogPosition.y + dialogPadding.y,
		nameColor.x, nameColor.y, nameColor.z, nameColor.w,
		0.0f,
		FONT_ALIGN::TOP_LEFT,
		scale
	);
	T_TEXT.TextBox(
		font,
		text.substr(0, (size_t)(timer / typeSpeed)),
		dialogLength,
		dialogPosition.x + dialogPadding.x, dialogPosition.y + dialogPadding.y + 60.0f,
		fontColor.x, fontColor.y, fontColor.z, fontColor.w,
		scale
	);
}

void Dialog::Clear()
{
	scale = 1.0f;
	text = "";
	name = "";
	dialogOwner = nullptr;
	Hide();
}

void Dialog::DrawDebugGUI()
{
}