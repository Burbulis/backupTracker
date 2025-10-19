#include "ReadFromBuff.h"
namespace shprotaBuff
{
/***************************************************/
	std::optional<std::uint32_t>
	ReadFrom::getAttribute(std::uint32_t index)
	{
		auto ret = _getAttr(index);
		if (!ret)
			return (ret);
		auto _ret = *ret;
		auto _tmp = static_cast<uint32_t>(index) | _ret >> 3U  ;
		position += sizeof(std::uint32_t);
		//buffer.erase(buffer.begin(), buffer.begin() + int32_max_length);
		return(_tmp);
	}
/***************************************************/
	std::optional<std::uint32_t>
		ReadFrom::_getAttr(std::uint32_t index)
	{
		std::vector< uint8_t > tmp;
		tmp.resize(sizeof(std::uint32_t));
		std::copy(buffer.begin() + position, buffer.begin()+ position + sizeof(std::uint32_t), tmp.begin());
		std::uint32_t ret = 0;
		decode32(ret, tmp);
		position += tmp.size();
		return(ret);
	}

/***************************************************/
	std::optional<uint32_t>
	ReadFrom::
	getTypeOf( uint32_t tag)
	{
		auto readed = _getAttr(tag);
		if (!readed)
			return (readed);
		auto _readed = *readed;
		return ((tag << 3U) ^ _readed);
	}
/***************************************************/
	bool 
	ReadFrom::IsFinal(void)
	{
		return (buffer.size() < sizeof(uint8_t));
	}
	
/***************************************************/
	size_t
	ReadFrom::getFieldDataSize(void)
	{
		uint64_t tmp = 0;
		//std::array<uint8_t, int32_max_length > src;
		std::vector<uint8_t> src;
		src.resize(int32_max_length);
		memset(src.data(), 0, src.size());
		memmove(src.data(), buffer.data() + position, src.size());
		decode64(tmp, src); // bytesToUint32(src);
		position += src.size();
		return(tmp);
	}
/***************************************************/
/***************************************************/
	std::optional<uint32_t>
	ReadFrom::extract32(uint32_t tag)
	{
		position = itearations[tag];
		auto switchType = getTypeOf(tag);
		if (!switchType)
			return (switchType);

		if ((uint32_t)pbf_wire_type::fixed32 != *switchType)
		{
			std::ostringstream _exception_info;	
		//			throw std::runtime_error("readed type mismatch!");
		    _exception_info << "readed type mismatch! switchType" << *switchType << " Tag = " << tag << 
			" file:line:func -  " <<	std::string(__FILE__)  <<  ":" << std::to_string(__LINE__) << ":" << std::string(&__FUNCTION__[0],&__FUNCTION__[strlen(__FUNCTION__)]);
			throw std::runtime_error(_exception_info.str());
		}
		std::vector<uint8_t> _buffer;
		_buffer.resize(sizeof(std::uint32_t));
		memmove(&_buffer[0], buffer.data() + position, sizeof(std::uint32_t));
		uint32_t ret = 0;
		decode32(ret, _buffer);
	//	position += sizeof(std::uint32_t);
		//itearations[tag] = position;
		return (ret);
	}


/***************************************************/
 std::optional<uint64_t>
	ReadFrom::extract64(uint32_t tag)
	{
	 position = itearations[tag];
		auto switchType = getTypeOf(tag);
		if (!switchType)
			return (switchType);
		if ((uint32_t)pbf_wire_type::fixed64 != *switchType)
		{
			std::ostringstream _exception_info;	
		    _exception_info << "readed type mismatch! switchType" << *switchType << " Tag = " << tag << 
			" file:line:func -  " <<	std::string(__FILE__)  <<  ":" << std::to_string(__LINE__) << ":" << std::string(&__FUNCTION__[0],&__FUNCTION__[strlen(__FUNCTION__)]);
			throw std::runtime_error(_exception_info.str());
		}
		uint64_t ret = 0;
		std::vector<uint8_t> _buffer;
		_buffer.resize(sizeof(std::uint64_t));
		memmove(&_buffer[0], buffer.data() + position, sizeof(std::uint64_t));
		decode64(ret, _buffer);
		return (ret);
	}
 /***************************************************/
 bool ReadFrom::operator<<(std::vector<uint8_t> buffer)
 {
	 enum { ID_COUNT = 1, _BUFFER = 2, CRC = 3 , END = CRC };
	 std::unique_ptr<shprotaBuff::ReadFrom>
		 _unpacker =
		 std::make_unique<shprotaBuff::ReadFrom>(buffer, CRC);
	 
	 if (*_unpacker->extract64(CRC) != crc(_unpacker->getBuffer(_BUFFER)))
		 throw std::runtime_error("crc32 input buffer error!");
	 init(_unpacker->getBuffer(_BUFFER), *_unpacker->extract32(ID_COUNT));
	 return true;
 }
/***************************************************/
 uint32_t ReadFrom::getCount(void)
 {
	 return countOf;
 }
 size_t
	ReadFrom::decode_varint(uint64_t& dst, std::array<char, max_varint_length > src)
	{
		return (decode_varint_impl(dst, src));
	}
/***************************************************/
	std::vector<uint8_t>
	ReadFrom::getBuffer(uint32_t tag)
	{
		position = itearations[tag];
		std::vector<uint8_t> _buffer;
		auto switchType = getTypeOf(tag);
		if (!switchType)
			return (_buffer);
		if ((std::uint32_t)pbf_wire_type::length_delimited != *switchType) 
		{
			std::ostringstream _exception_info;	
		    _exception_info << "readed type mismatch! switchType" << *switchType << " Tag = " << tag << 
			" file:line:func -  " <<	std::string(__FILE__)  <<  ":" << std::to_string(__LINE__) << ":" << std::string(&__FUNCTION__[0],&__FUNCTION__[strlen(__FUNCTION__)]);
			throw std::runtime_error(_exception_info.str());
		}
		//pbf_wire_type::length_delimited
			//throw std::runtime_error("readed type mismatch!");
			const size_t size = getFieldDataSize();
		_buffer.resize(size);
		memmove(&_buffer[0], buffer.data() + position, size);
	
		return (_buffer);
	}
	/*************************************************************/
	bool ReadFrom::next(uint32_t tag)
	{
		itearations[tag] = position;
		std::vector<uint8_t> _buffer;
		auto switchType = getTypeOf(tag);
		if (!switchType)
			return false;
		size_t size = sizeof(std::uint32_t);	
		if ((uint32_t)pbf_wire_type::fixed64 == *switchType)
			size = sizeof(std::uint64_t);
		if ((std::uint32_t)pbf_wire_type::length_delimited == *switchType)
			size = getFieldDataSize();
		position += size;
		return true;
	}
	/*************************************************************/
	void ReadFrom::init(std::vector<uint8_t> _buffer, uint32_t _countOf)
	{
		buffer = _buffer;
		countOf = _countOf;
			for (uint32_t i = 1; i <= countOf; ++i)
				next(i);
	}
	/*************************************************************/
	void ReadFrom::init(uint32_t countOf)
	{
		for (uint32_t i = 1; i <= countOf; ++i)
			next(i);
	}
	/*************************************************************/
	std::string ReadFrom::getString(uint32_t tag)
	{
		std::vector<uint8_t> _Tmp = getBuffer(tag);
		std::string str(_Tmp.begin(), _Tmp.end());
		return (str);
	}
	/*************************************************************/
	inline  size_t ReadFrom::decode16(uint16_t& dst, std::vector<uint8_t> src)
	{
		uint16_t val = 0;
		size_t i = 0;
		unsigned int shift = 0;
		const size_t final = src.size();
		for (size_t i = 0; i < final; ++i) {
			val |= static_cast<uint16_t>(src[i]) << (8 * i);
		}
		dst = val;
		return ++i;
	}
/*************************************************************/
	inline  size_t ReadFrom::decode32(uint32_t& dst, std::vector<uint8_t> src)
	{
		uint32_t val = 0;
		size_t i = 0;
		unsigned int shift = 0;
		const size_t final = src.size();
		for (size_t i = 0; i < final; ++i) {
			val |= static_cast<uint32_t>(src[i]) << (8 * i);
		}
		dst = val;
		return ++i;
	}
/*************************************************************/
	inline  size_t ReadFrom::decode64(uint64_t& dst, std::vector<uint8_t> src)
	{
		uint64_t val = 0;
		size_t i = 0;
		unsigned int shift = 0;
		const size_t final = src.size();
		for (size_t i = 0; i < final; ++i) {
			val |= static_cast<uint64_t>(src[i]) << (8 * i);
		}
		dst = val;
		return ++i;
	}
/*************************************************************/
	inline size_t ReadFrom::decode_varint_impl(uint64_t& dst, std::array<char, max_varint_length > src)
	{
		uint64_t val = 0;
		size_t  i = 0;
		unsigned int shift = 0;
		const size_t final = buffer.size();
		for (i = 0; i < final && buffer[i] < 0; ++i, shift += 7)
			val |= (static_cast<uint64_t>(buffer[i]) & 0x7fU) << shift;
		dst = val | static_cast<uint64_t>(buffer[i]) << shift;
		return ++i;
	}
/*************************************************************/
	uint32_t ReadFrom::bytesToUint32(std::array<uint8_t, int32_max_length> bytes)
	{
		uint32_t value = 0;
		value |= static_cast<uint32_t>(bytes[0]) << 24;
		value |= static_cast<uint32_t>(bytes[1]) << 16;
		value |= static_cast<uint32_t>(bytes[2]) << 8;
		value |= static_cast<uint32_t>(bytes[3]);
		return value;
	}

}