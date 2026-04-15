#include "WriteToBuff.h"
#include <boost/crc.hpp>
namespace shprotaBuff
{
	
	void writeTo::add(std::string name,std::string str)
	{
		std::vector<uint8_t> tmp(str.begin(),str.end());
		add(name,tmp);
		return;
	}

	void 
	writeTo::add(std::string name,uint64_t val)
	{
		++Id;
		nameAndId naid;
		naid = { name,Id };
		protoInfo.push_back(naid);
		setAttributes(getAttribute(Id, pbf_wire_type::fixed64));
		auto tmp = encode_to_array(static_cast<uint64_t>(val));
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
		return;
	}

	void 
	writeTo::add(std::string name,uint32_t val)
	{
		constexpr auto min = std::numeric_limits<uint32_t>::min();
		constexpr auto max = std::numeric_limits<uint32_t>::max();
		++Id;
		if (((min <= val) &&
			(val >= max)))
			throw std::runtime_error("value limit Error!");
		nameAndId naid;
		naid = { name,Id };
		protoInfo.push_back(naid);
		setAttributes(getAttribute(Id, pbf_wire_type::fixed32));
		auto tmp = encode_to_array(static_cast<uint32_t>(val));
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}
	

	void 
	writeTo::add(std::string name, std::vector<uint8_t> tmp)
	{
		++Id;
		nameAndId naid;
		naid = { name,Id };
		protoInfo.push_back(naid);
		setAttributes(getAttribute(Id, pbf_wire_type::length_delimited));
		auto _size_buff = encode_to_array(static_cast<uint32_t>(tmp.size()));
		_buffer.insert(_buffer.end(), _size_buff.begin(), _size_buff.end());
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}

	void
	writeTo::add(std::string name, std::tm& dateTime)
	{
		++Id;
		nameAndId naid;
		naid = { name,Id };
		protoInfo.push_back(naid);
		time_t _dateTime = std::mktime(&dateTime);
		setAttributes(getAttribute(Id, pbf_wire_type::datetime));
		auto tmp = encode_to_array(static_cast<uint64_t>(_dateTime));
		_buffer.insert(_buffer.end(), tmp.begin(), tmp.end());
	}


	void 
	writeTo::add(std::string name,uint8_t* tmp, size_t size)
	{
		std::vector< uint8_t > _tmp(&tmp[0], &tmp[size]);
		add(name,_tmp);
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
		writeTo::operator ()()
	{
		std::unique_ptr<shprotaBuff::writeTo> dataDescTmp =
			std::make_unique<shprotaBuff::writeTo>();
		std::unique_ptr<shprotaBuff::writeTo> dataDesc =
			std::make_unique<shprotaBuff::writeTo>();

		for (size_t i = 0; i < protoInfo.size(); ++i)
		{
			std::unique_ptr<shprotaBuff::writeTo> desc =
				std::make_unique<shprotaBuff::writeTo>();
			desc->add("NAME",protoInfo[i].name);
			desc->add("ID", protoInfo[i].Id);
			dataDescTmp->add("desc", desc->getBuffer() );
		}
		dataDesc->add("totalDesc", dataDescTmp->getBuffer() );
		dataDesc->add("ID", dataDescTmp->getId());
				
		enum { DESC_S =1, ID_COUNT = 2 , _BUFFER = 3, CRC = 4};
		std::unique_ptr<shprotaBuff::writeTo> packer =
			std::make_unique<shprotaBuff::writeTo>();
		packer->add("DESCS_BUFFER", dataDesc->getBuffer());
		packer->add("ID",Id);
		packer->add("BUFFER",_buffer);
		packer->add("CRC",static_cast<std::uint32_t>(crc(_buffer)));
		return (packer->getBuffer());
	}


}