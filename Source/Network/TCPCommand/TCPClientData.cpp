//! file TCPClientData.cpp
//! @note 

#include "TCPClientData.h"
#include <vector>
#include <iostream>

#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@brief		クライアントデータ同期受信
		@param[in]	size データトータルサイズ
		@return		成功判定
	*//***************************************************************************/
	bool TCPClientData::Receive(size_t size)
	{
		uint64_t member_num = 0;
		if (m_pcontroller->GetTcpSocket()->Receive(&member_num, sizeof(uint64_t)))
		{
			for (uint64_t i = 0; i < member_num; i++)
			{
				PlayerCharacter* player = this->ReceiveClient();
				if (player == nullptr)
				{
					return false;
				}
				else {
					std::cout << player->GetName() << std::endl;
				}
			}
			return true;
		}
		return false;
	}
}
