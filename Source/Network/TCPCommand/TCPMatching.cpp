//! @file TCPMatching.cpp
//! @note

#include "TCPMatching.h"
#include "UI/Widget/WidgetMatching.h"

namespace Online
{
	/**************************************************************************//**
		@brief		マッチング開始送信開始
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPMatchingStart::Send(void* data)
	{
		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, 0);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	/**************************************************************************//**
		@brief		マッチング終了送信開始
		@param[in]	data データ参照ポインタ
		@return		成功判断
	*//***************************************************************************/
	bool TCPMatchingEnd::Send(void* data)
	{
		std::vector<uint8_t> buffer;
		CreateHeaderBuffer(buffer, m_cmd, 0);

		return m_pcontroller->GetTcpSocket()->Send(buffer.data(), buffer.size()) >= 0;
	}

	bool TCPMatchingUpdate::Receive(size_t size)
	{
		MATCHING_DATA matching_data;
		if (m_pcontroller->GetTcpSocket()->Receive(&matching_data, sizeof(MATCHING_DATA)) > 0)
		{
			WidgetMatching* ui = m_pcontroller->GetMathcingUI();
			if (ui != nullptr)
			{
				ui->SetMaxPlayerNum(matching_data.max_client_num);
				ui->SetPlayerNum(matching_data.client_num);
				return true;
			}
		}
		return false;
	}
}


