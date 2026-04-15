#pragma once
#ifndef SHPROTA_READ
#define SHPROTA_READ
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <sstream>
#include <tuple>
#include <set>
#include "WriteToBuff.h"
//#include "base.hpp"

namespace shprotaBuff
{
	constexpr const int8_t max_varint_length = (sizeof(uint64_t) * 8 / 7) + 1;
	constexpr const int8_t int32_max_length = 4;
	class ReadFrom:protected baseBuff
	{
		uint32_t countOf;
		std::vector<uint8_t> buffer;
		size_t decode_varint_impl(uint64_t& dst, std::array<char, max_varint_length > src);
		size_t decode_varint(uint64_t& dst, std::array<char, max_varint_length > src);
		uint32_t bytesToUint32(std::array<uint8_t, int32_max_length> bytes);
		inline   size_t decode64(uint64_t& dst, std::vector<uint8_t> src);
		inline   size_t decode32(uint32_t& dst, std::vector<uint8_t> src);
		inline   size_t decode16(uint16_t& dst, std::vector<uint8_t> src);
		std::optional<uint32_t> getTypeOf(uint32_t tag);
		std::optional<std::uint32_t> _getAttr(std::uint32_t index);
		size_t position;
		std::unordered_map<size_t,size_t> itearations;
		void init(std::vector<uint8_t> _buffer, uint32_t countOf);
		void init(uint32_t countOf);
	public:
		ReadFrom(std::vector<uint8_t> _buffer,uint32_t countOf) 
			:countOf(0), position(0)
		{	
			init(_buffer,countOf);
		}
		ReadFrom(std::vector<uint8_t> buffer)
			:countOf(0),position(0)
		{ operator<<(buffer); 
		}
	//	std::unordered_map<std::string, uint32_t> umapInfo;
		std::unordered_map<std::string, uint32_t> getUmap(void);
		void getDesc(std::vector<uint8_t> buffer);
		bool operator<<(std::vector<uint8_t> buffer);
		uint32_t getCount(void);
		std::optional <uint64_t> extract64(uint32_t tag);
		std::optional<uint64_t>  extract64(std::string tagName);
		std::optional<uint32_t>  extract32(uint32_t tag);
		std::optional<uint32_t>	 extract32(std::string tagName);
		std::optional<std::tm>   getDateTime(int32_t tag);
		std::optional<std::tm>   getDateTime(std::string tagName);
		std::optional<std::uint32_t>	getAttribute(std::uint32_t index);
		std::string getString(uint32_t tag);
		std::string getString(std::string tagName);
		size_t getFieldDataSize(void);
		bool checkField(std::string tagName);
		std::vector<uint8_t> getBuffer(uint32_t tag);
		std::vector<uint8_t> getBuffer(std::string tagName);
		bool next(uint32_t tag);
		bool IsFinal(void);
	};
}
#endif