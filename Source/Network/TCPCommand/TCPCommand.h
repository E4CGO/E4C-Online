//! @file TCPCommand.h
//! @note 

#ifndef __ONLINE_TCP_COMMAND_H__
#define __ONLINE_TCP_COMMAND_H__

#include "Network/OnlineController.h"

#include "PlayerCharacterData.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

namespace Online
{
	/**************************************************************************//**
		@class		TCPCommand
		@brief		TCP処理の基底クラス
		@par		[説明]
						クラスサ
	*//***************************************************************************/
	class OnlineController;

	class TCPCommand
	{
	public:
		// コンストラクタ（引数付き）
		TCPCommand(OnlineController* controller, uint8_t cmd) : m_pcontroller(controller), m_cmd(cmd) {};
		// デストラクタ
		virtual ~TCPCommand() = default;

		// データ受信
		virtual bool Receive(size_t size) = 0;
		// データ送信
		virtual bool Send(void* data) { return true; };

	protected:
		//TCP送信用ヘッダ
		void CreateHeaderBuffer(std::vector<uint8_t>& buffer, uint8_t cmd, uint32_t size);

		// Rustの文字を受信
		bool ReceiveString(std::string& string) {
			uint64_t str_length = 0;
			if (m_pcontroller->GetTcpSocket()->Receive(&str_length, sizeof(uint64_t)) > 0)
			{
				char* str = new char[str_length + 1];
				ZeroMemory(str, str_length + 1);
				if (m_pcontroller->GetTcpSocket()->Receive(str, str_length) > 0)
				{
					string = str;
					delete[] str;
					return true;
				}
				delete[] str;
			}
			return false;
		}

		PlayerCharacter* ReceiveClient()
		{
			uint64_t client_id = 0;
			if (m_pcontroller->GetTcpSocket()->Receive(&client_id, sizeof(uint64_t)) > 0)
			{
				// 外見パーツ
				uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM] = {};
				if (m_pcontroller->GetTcpSocket()->Receive(&appearance, PlayerCharacterData::APPEARANCE_PATTERN::NUM) > 0)
				{
					std::string name;
					if (ReceiveString(name))
					{
						PlayerCharacter* player = PlayerCharacterManager::Instance().UpdatePlayerData(client_id, name.c_str(), appearance);
						player->SetFriction(0.0f);
						return player;
					}
				}
			}

			return nullptr;
		}
	protected:
		OnlineController* m_pcontroller;
		uint8_t m_cmd;
	};

	/**************************************************************************//**
		@class		TCPNone
		@brief		ダミークラス
		@par		[説明]
	*//***************************************************************************/
	class TCPNone : public TCPCommand
	{
	public:
		// コンストラクタ（引数付き）
		TCPNone(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

		// データ受信 
		bool Receive(size_t size) override {
			if (size > 0)
			{
				std::string buffer;
				m_pcontroller->GetTcpSocket()->Receive(&buffer, size);
			}
			return true;
		};

	};
}

#endif