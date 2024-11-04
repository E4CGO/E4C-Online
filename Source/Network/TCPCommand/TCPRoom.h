//! file TCPRoom.h
//! @note 

#ifndef __ONLINE_TCP_ROOM_H__
#define __ONLINE_TCP_ROOM_H__

#include <string>
#include "TCPCommand.h"

namespace Online
{
    /**************************************************************************//**
		@class		TCPRoomIn
		@brief		入室処理処理クラス
		@par		[説明]
						入室したキャラの処理
	*//***************************************************************************/
    class TCPRoomIn : public TCPCommand
    {
    public:
        TCPRoomIn(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};
        
        // データ受信
        bool Receive(size_t size) override;
    };

    /**************************************************************************//**
        @class		TCPRoomOut
        @brief		退室処理処理クラス
        @par		[説明]
                        退室したキャラの処理
    *//***************************************************************************/
    class TCPRoomOut : public TCPCommand
    {
    public:
        TCPRoomOut(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

        // データ受信
        bool Receive(size_t size) override;
    };

    class TCPRoomNew : public TCPCommand
    {
    };
}
#endif