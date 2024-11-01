//! @file UDPCommand.cpp
//! @note 

#include "UDPCommand.h"
#include "TAKOEngine/Tool/U8Buffer.h"

namespace Online
{
	/**************************************************************************//**
		@brief		UDP送信用ヘッダ
		@param[in]	buffer	参照送信用バッファ
		@param[in]	cmd		送信コマンド
		@param[in]	size	データサイズ
	*//***************************************************************************/
	void UDPCommand::CreateHeaderBuffer(std::vector<uint8_t>& buffer, uint8_t cmd, uint32_t size)
	{
		buffer.clear();
		// クライアントID
		U8Buffer::Insert(buffer, m_pcontroller->GetId());
		// トークン
		U8Buffer::Insert(buffer, m_pcontroller->GetToken());
		// コマンド
		buffer.push_back(cmd);
		// データサイズ
		U8Buffer::Insert(buffer, size);
	}
}
