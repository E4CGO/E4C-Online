//! @file WidgetMatching.h
//! @note

#ifndef __INCLUDE_WIDGET_MATCHING_H__
#define __INCLUDE_WIDGET_MATCHING_H__

#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetButtonImage.h"
#include "UI/Widget/WidgetImage.h"

namespace Online {
	class OnlineController;
}
class Teleporter;

class WidgetMatching : public WidgetButtonImage
{
public:
	// コンストラクタ
	WidgetMatching(Online::OnlineController* onlineController, Teleporter* teleporter);
	// デストラクタ
	~WidgetMatching();

	enum STATE : uint8_t {
		INIT = 0,			// 初期化
		MATCHING,			// マッチング中
		FULL				// 満員、カウントダウン開始
	};

	void Update(float elapsedTime);

	void RenderDX12(const RenderContextDX12& rc) override;

	void Callback();
	void SetMaxPlayerNum(const uint8_t num) { m_maxPlayerNum = num; }
	void SetPlayerNum(const uint8_t num) { m_playerNum = num; }

	Teleporter* GetTeleporter() { return m_pTeleporter; }
private:
	Teleporter* m_pTeleporter;

	uint8_t m_state = 0;

	Online::OnlineController* m_pOnlineController;
	uint8_t m_maxPlayerNum = 1;
	uint8_t m_playerNum = 0;

	const float m_countTime = 5.0f;
	float m_timer = 0.0f;

	std::shared_ptr<SpriteDX12> m_dungeon;
	std::shared_ptr<SpriteDX12> m_enter;

	WidgetText* m_matchingText;
	std::string m_textToRender;
	float m_textScale;
};

#endif // !__INCLUDE_WIDGET_MATCHING_H__
