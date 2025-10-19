#pragma once

#ifndef SHPROTA_WRITE
#define SHPROTA_WRITE

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <memory>
#include "baseBuff.hpp"
namespace shprotaBuff
{
	class writeTo: protected baseBuff
	{
		std::vector<uint8_t> _buffer;
		enum class pbf_wire_type : uint32_t {
			varint = 0, // int32/64, uint32/64, sint32/64, bool, enum
			fixed32 = 1,
			fixed64 = 2, // fixed64, sfixed64, double
			length_delimited = 3, // string, bytes, nested messages, packed repeated fields
			 // fixed32, sfixed32, float
			unknown = 99 // used for default setting in this library
		};
		uint32_t Id;

		//uint32_t getTypeOf(uint32_t readed, uint32_t tag);

		template
			<
			typename T = uint64_t,
			typename TBuffer = std::array<uint8_t, sizeof(T)>//std::vector<uint8_t>
			>
		TBuffer encode_to_array(T value)
		{
			TBuffer seq;
			memset(seq.data(), 0, seq.size());
			std::vector<uint8_t> bytes(sizeof(T));
			for (size_t i = 0; i < sizeof(T); ++i) 
				seq[i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
			return seq;
		}
		//uint64_t crc(void);
	public:
		std::vector <uint8_t>	operator ()();
		void add(uint32_t val);
		void add(uint64_t val);
		void add(std::vector<uint8_t> tmp);
		void add(std::string str);
		void add(uint8_t* tmp, size_t size);
		void setAttributes(uint32_t attribute);
		uint32_t getId(void) const;
		uint32_t getAttribute(uint32_t tag, pbf_wire_type type);
		std::vector<uint8_t>& getBuffer(void);
	};
};
#endif