//! @file Widget.h
//! @note

#ifndef __INCLUDED_UI_WIDGET__
#define __INCLUDED_UI_WIDGET__

#include <DirectXMath.h>
#include "TAKOEngine/Rendering/SpriteDX12.h"
#include "TAKOEngine/Rendering/RenderContext.h"
/**************************************************************************//**
	@class
	@brief	UIウィジェット基底クラス
	@par	[説明]
*//***************************************************************************/
class Widget
{
public:
	// コンストラクタ
	Widget() {};
	// デストラクタ
	virtual ~Widget() {};

	// 更新処理
	virtual void Update(float elapsedTime) {};
	// 描画処理
	virtual void Render(const RenderContext& rc) = 0;

	virtual void RenderDX12(const RenderContextDX12& rc) = 0;
public:
	// アクセス

	virtual void SetPosition(const DirectX::XMFLOAT2& position) { this->m_position = position; }
	const DirectX::XMFLOAT2& GetPosition() const { return m_position; }
	virtual void SetSize(const DirectX::XMFLOAT2& size) { this->m_size = size; }
	const DirectX::XMFLOAT2& GetSize() const { return m_size; }
protected:
	DirectX::XMFLOAT2 m_position = {};
	DirectX::XMFLOAT2 m_size = {};
};

#endif // !__INCLUDED_UI_WIDGET__