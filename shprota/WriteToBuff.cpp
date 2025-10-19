#include "WriteToBuff.h"
#include <boost/crc.hpp>
namespace shprotaBuff
{
	
	void writeTo::add(std::string str)
	{
		std::vector<uint8_t> tmp(str.begin(),str.end());
		add(tmp);
		return;
	}

	void 
	writeTo::add(uint64_t val)
	{
		++Id;
		setAttributes(getAttribute(Id, pbf_wire_type::fixed64));
		auto tmp = encode_to_array(static_cast<uint64_t>(val));
	//	auto _tmp = encode_to_array(static_cast<uint32_t>(tmp.size()));
	//	_buffer.insert(_buffer.end(), _tmp.begin(), _tmp.end());
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
		return;
	}

	void 
	writeTo::add(uint32_t val)
	{
		constexpr auto min = std::numeric_limits<uint32_t>::min();
		constexpr auto max = std::numeric_limits<uint32_t>::max();
		++Id;
		if (((min <= val) &&
			(val >= max)))
			throw std::runtime_error("value limit Error!");
		setAttributes(getAttribute(Id, pbf_wire_type::fixed32));
	//	auto _tmp = encode_to_array(static_cast<uint32_t>(sizeof(val)));
		auto tmp = encode_to_array(static_cast<uint32_t>(val));
//		_buffer.insert(_buffer.end(), _tmp.begin(), _tmp.end());
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}
	

	void 
	writeTo::add(std::vector<uint8_t> tmp)
	{
		++Id;
		setAttributes(getAttribute(Id, pbf_wire_type::length_delimited));
		auto _size_buff = encode_to_array(static_cast<uint32_t>(tmp.size()));
		_buffer.insert(_buffer.end(), _size_buff.begin(), _size_buff.end());
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}

	void 
	writeTo::add(uint8_t* tmp, size_t size)
	{
		std::vector< uint8_t > _tmp(&tmp[0], &tmp[size]);
		add(_tmp);
	}

	std::vector<uint8_t>& 
	writeTo::getBuffer(void)
	{
		return _buffer;
	}
	
	uint32_t 
	writeTo::getAttribute(uint32_t tag, pbf_wire_type type)
	{
		uint32_t test = (tag << 3U) | static_cast<uint32_t>(type);
	//	auto _type = getTypeOf(test, tag);
		return (test);
	}
	uint32_t writeTo::getId(void) const
	{
		return Id;
	}

	void 
	writeTo::setAttributes(std::uint32_t attribute)
	{
		auto tmp = encode_to_array(static_cast<std::uint32_t>(attribute));
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}



	std::vector <uint8_t>
		writeTo::operator () ()
	{
		enum {ID_COUNT = 1 , _BUFFER = 2, CRC = 3};
		std::unique_ptr<shprotaBuff::writeTo> packer =
			std::make_unique<shprotaBuff::writeTo>();
		packer->add(Id);
		packer->add(_buffer);
		packer->add(crc(_buffer));
		return (packer->getBuffer());
	}


}