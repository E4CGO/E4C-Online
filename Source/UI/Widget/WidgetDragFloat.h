//! @file WidgetDragFloat.h
//! @note

#ifndef __WIDGET_DRAG_FLOAT__
#define __WIDGET_DRAG_FLOAT__

#include <string>
#include <memory>

#include "TAKOEngine/Rendering/Sprite.h"

#include "UI/Widget/WidgetButton.h"

/**************************************************************************//**
	@class	WidgetDragBar
	@brief	値を調整用ドラッグバー基底クラス
	@par	[説明]

*//***************************************************************************/
class WidgetDragBar : public WidgetButton
{
public:
	// コンストラクタ
	WidgetDragBar(float rate = 0.0f);
	// デストラクタ
	~WidgetDragBar() {};
	// 描画処理
	void Render(const RenderContext& rc) override;
	// 描画処理
	void RenderDX12(const RenderContextDX12& rc) override;
	// 押しつづけ勝利
	void OnPress() override;
	// 現在%を取得
	float GetRate() { return m_rate; };
	// 現在%を設定
	float SetRate(float rate) { this->m_rate = rate; }
private:
	// 現在%値
	float m_rate;

	// UI画像スプライト
	std::shared_ptr<Sprite> m_scrollBar;
	std::shared_ptr<Sprite> m_scrollBtn;
};

/**************************************************************************//**
	@class	WidgetDragFloat
	@brief	Float値調整用ドラッグGUI
	@par	[説明]
*//***************************************************************************/
class WidgetDragFloat : public Widget
{
public:
	// コンストラクタ
	WidgetDragFloat(const char* label, float* value, float minValue, float maxValue);
	// デストラクタ
	~WidgetDragFloat() { delete m_pBar; };
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render(const RenderContext& rc) override;
	// 描画処理
	void RenderDX12(const RenderContextDX12& rc) override;
	// 位置調整
	void SetPosition(const DirectX::XMFLOAT2& position) override;
	// サイズ調整
	void SetSize(const DirectX::XMFLOAT2& size) override;
private:
	// ラベル
	std::string m_label;
	// float値参照ポインタ
	float* m_pValue;
	// 最小値
	float m_minValue;
	// 最大値
	float m_maxValue;
	// ドラッグバー参照ポインタ
	WidgetDragBar* m_pBar;
};
#endif // !__WIDGET_DRAG_FLOAT__
