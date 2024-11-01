//! @file TCPCommand.cpp
//! @note 

#include "TCPCommand.h"
#include "TAKOEngine/Tool/U8Buffer.h"

namespace Online
{
	/**************************************************************************//**
		@brief		TCP送信用ヘッダ
		@param[in]	buffer	参照送信用バッファ
		@param[in]	cmd		送信コマンド
		@param[in]	size	データサイズ
	*//***************************************************************************/
	void TCPCommand::CreateHeaderBuffer(std::vector<uint8_t>& buffer, uint8_t cmd, uint32_t size)
	{
		buffer.clear();
		buffer.push_back(cmd);
		U8Buffer::Insert(buffer, size);
	}

}
