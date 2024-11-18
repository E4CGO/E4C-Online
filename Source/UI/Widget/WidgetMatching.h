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
	// �R���X�g���N�^
	WidgetMatching(Online::OnlineController* onlineController, Teleporter* teleporter);
	// �f�X�g���N�^
	~WidgetMatching();

	enum STATE : uint8_t {
		INIT = 0,			// ������
		MATCHING,			// �}�b�`���O��
		FULL				// �����A�J�E���g�_�E���J�n
	};

	void Update(float elapsedTime);

	void Callback();
	void SetMaxPlayerNum(const uint8_t num) { m_maxPlayerNum = num; }
	void SetPlayerNum(const uint8_t num) { m_playerNum = num; }
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

