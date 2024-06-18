#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "WidgetCrosshair.h"
#include <iostream>

static WidgetCrosshair* instance = nullptr;
// インスタンス取得
WidgetCrosshair& WidgetCrosshair::Instance()
{
	return *instance;
}

WidgetCrosshair::WidgetCrosshair() : WidgetImage("Data/Sprites/crosshair122.png")
{
	// インスタンスポインタ設定
	instance = this;

	size.x = size.y = SCREEN_W * 0.04f;
	position = DirectX::XMFLOAT2{ SCREEN_W * 0.5f - size.x * 0.5f, SCREEN_H * 0.5f - size.y * 0.5f };
	color.w = 0.0f;
}

void WidgetCrosshair::Update(float elapsedTime)
{
	angle += DirectX::XMConvertToRadians(90) * elapsedTime;
	if (angle > DirectX::XM_PI) angle -= DirectX::XM_2PI;
	if (angle < -DirectX::XM_PI) angle += DirectX::XM_2PI;

	if (!enable)
	{
		color.w -= elapsedTime * 2.0f;
		if (color.w < 0.0f)  color.w = 0.0f;
	}
	else
	{
		color.w += elapsedTime * 2.0f;
		if (color.w > 1.0f)  color.w = 1.0f;
	}
}