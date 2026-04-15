#include <boost/crc.hpp>
#include <vector>
#include <unordered_map>
namespace shprotaBuff
{
	class baseBuff
	{
	protected:
			struct nameAndId
			{
				std::string name;
				uint32_t Id;
			};
			std::vector<nameAndId> protoInfo;
			std::unordered_map<std::string, uint32_t> umapInfo;

			enum class pbf_wire_type : uint32_t {
			varint   = 0, // int32/64, uint32/64, sint32/64, bool, enum
			fixed32  = 1,
			fixed64  = 2, // fixed64, sfixed64, double
			datetime = 3,
			length_delimited = 4, // string, bytes, nested messages, packed repeated fields
			 // fixed32, sfixed32, float
			unknown = 99 // used for default setting in this library
		};
		size_t crc(std::vector<uint8_t > buff)
		{
			boost::crc_32_type result;
			result.process_bytes(buff.data(), buff.size());
			return result.checksum();
		}

	};
}