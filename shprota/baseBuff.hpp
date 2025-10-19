#include <boost/crc.hpp>
#include <vector>
namespace shprotaBuff
{
	class baseBuff
	{
	protected:
		size_t crc(std::vector<uint8_t > buff)
		{
			boost::crc_32_type result;
			result.process_bytes(buff.data(), buff.size());
			return result.checksum();
		}

	};
}