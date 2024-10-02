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
	//‰Šú‰»
	void Initialize();

	//MRTİ’è
	void SetDeferredRTV();

	//•`‰æ
	void Render();

	//GUI
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> sprite;
};

