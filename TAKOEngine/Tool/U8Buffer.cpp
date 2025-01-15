//! @file U8Buffer.cpp
//! @note 

#include "U8Buffer.h"
#include "TAKOEngine/Network/WinSock2Wrapper.h"
#include <WS2tcpip.h>

namespace U8Buffer {
	/**************************************************************************//**
		@brief		u8データをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	u8データ
		@return		なし
	*//***************************************************************************/
	void Insert(std::vector<uint8_t>& buffer, uint8_t value)
	{
		buffer.push_back(value);
	}
	/**************************************************************************//**
		@brief		u32データをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	u32データ
		@return		なし
	*//***************************************************************************/
	void Insert(std::vector<uint8_t>& buffer, uint32_t value)
	{
		uint32_t size_network_order = htonl(value); // ネットワークバイトオーダーに変換
		buffer.insert(
			buffer.end(),
			reinterpret_cast<uint8_t*>(&size_network_order),
			reinterpret_cast<uint8_t*>(&size_network_order) + sizeof(value)
		);
	}
	/**************************************************************************//**
		@brief		u64データをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	u64データ
		@return		なし
	*//***************************************************************************/
	void Insert(std::vector<uint8_t>& buffer, uint64_t value)
	{
		uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
		uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFF));

		buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&high_part), reinterpret_cast<uint8_t*>(&high_part) + sizeof(high_part));
		buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&low_part), reinterpret_cast<uint8_t*>(&low_part) + sizeof(low_part));
	}
	/**************************************************************************//**
		@brief		floatデータをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	floatデータ
		@return		なし
	*//***************************************************************************/
	void Insert(std::vector<uint8_t>& buffer, float value)
	{
		uint32_t temp;
		memcpy(&temp, &value, sizeof(float));  // float を uint32_t にコピー
		temp = htonl(temp);                    // ネットワークバイトオーダーに変換
		buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&temp), reinterpret_cast<uint8_t*>(&temp) + sizeof(temp));
	}
	/**************************************************************************//**
		@brief		stringデータをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	stringデータ
		@return		なし
	*//***************************************************************************/
	void Insert(std::vector<uint8_t>& buffer, const std::string& value)
	{
		buffer.insert(buffer.end(), value.begin(), value.end());
	}
	/**************************************************************************//**
		@brief		utf8 stringデータをu8バッファに挿入
		@param[in]	buffer	参照型u8バッファ
		@param[in]	value	utf8 stringデータ
		@return		なし
	*//***************************************************************************/
	void InsertU8(std::vector<uint8_t>& buffer, const std::string& value)
	{
		buffer.insert(buffer.end(), value.begin(), value.end());
	}
}