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
	//������
	void Initialize();

	//MRT�ݒ�
	void SetDeferredRTV();

	//�`��
	void Render();

	//GUI
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> sprite;
};

