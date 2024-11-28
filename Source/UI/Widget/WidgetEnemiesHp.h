#pragma once

#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/GUI/Widget.h"
#include "TAKOEngine/Rendering/FrameBufferManager.h"

class WidgetEnemiesHp : public Widget
{
public:
	WidgetEnemiesHp(float width = 100.0f, float height = 10.0f);
	WidgetEnemiesHp(DirectX::XMFLOAT2 size);

	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;
private:
	std::shared_ptr<Sprite> gauge;
	std::shared_ptr<SpriteDX12> m_gaugeDX12;

	// フレームバッファマネージャー
	FrameBufferManager * m_frameBuffer = nullptr;
};
