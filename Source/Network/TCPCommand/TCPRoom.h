//! file TCPRoom.h
//! @note 

#ifndef __ONLINE_TCP_ROOM_H__
#define __ONLINE_TCP_ROOM_H__

#include <string>
#include "TCPCommand.h"

namespace Online
{
    struct RoomData
    {
        uint32_t owner_id;
        uint8_t& tree;
    };

    /**************************************************************************//**
		@class		TCPRoomIn
		@brief		入室命令処理クラス
		@par		[説明]
						入室したキャラの処理
	*//***************************************************************************/
    class TCPRoomIn : public TCPCommand
    {
    public:
        TCPRoomIn(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

        // データ受信
        bool Receive(size_t size) override;
        // データ送信
        bool Send(void* data) override;
    };

    /**************************************************************************//**
        @class		TCPRoomNew
        @brief		新しい命令処理クラス
        @par		[説明]
                        入室したキャラの処理
    *//***************************************************************************/
    class TCPRoomNew : public TCPCommand
    {
    public:
        TCPRoomNew(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

        // データ受信
        bool Receive(size_t size) override;
        // データ送信
        bool Send(void* data) override;
    };

    /**************************************************************************//**
        @class		TCPRoomOut
        @brief		退室命令処理クラス
        @par		[説明]
                        退室したキャラの処理
    *//***************************************************************************/
    class TCPRoomOut : public TCPCommand
    {
    public:
        TCPRoomOut(OnlineController* controller, uint8_t cmd) : TCPCommand(controller, cmd) {};

        // データ受信
        bool Receive(size_t size) override;
        //// データ送信
        //bool Send(void* data) override;
    };
}
#endif