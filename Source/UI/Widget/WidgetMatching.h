//! @file WidgetMatching.h
//! @note 

#ifndef __INCLUDE_WIDGET_MATCHING_H__
#define __INCLUDE_WIDGET_MATCHING_H__

#include "UI/Widget/WidgetButtonText.h"
namespace Online {
	class OnlineController;
}
class Teleporter;

class WidgetMatching : public WidgetButtonText
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
};

#endif // !__INCLUDE_WIDGET_MATCHING_H__

