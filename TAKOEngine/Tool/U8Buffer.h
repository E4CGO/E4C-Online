//! @file U8Buffer.h
//! @note 

#ifndef __INCLUDE_U8_BUFFER_H__
#define __INCLUDE_U8_BUFFER_H__

#include <vector>
#include <string>

namespace U8Buffer {
	void Insert(std::vector<uint8_t>& buffer, uint8_t value);
	void Insert(std::vector<uint8_t>& buffer, uint32_t value);
	void Insert(std::vector<uint8_t>& buffer, uint64_t value);
	void Insert(std::vector<uint8_t>& buffer, float value);
	void Insert(std::vector<uint8_t>& buffer, const std::string& value);
	void InsertU8(std::vector<uint8_t>& buffer, const std::string& value);
}

#endif



