#pragma once

#include <d3d11.h>
#include <imgui.h>
#include <memory>
#include "Misc.h"

#include "Sprite.h"
#include "TAKOEngine\Rendering\DeferredRendering.h"

class DeferredRendering
{
public:
	//初期化
	void Initialize();

	//MRT設定
	void SetDeferredRTV();

	//描画
	void Render();

	//GUI
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> sprite;
};

