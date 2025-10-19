#ifndef UID_AND_ID_CLASS
#define UID_AND_ID_CLASS

#include <vector>
#include <string>
#include "_log.hpp"
#include "typedef.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace types;
class uid_sAndId_s
{
public:

    static
        std::string
        Newuid()
    {
        boost::uuids::uuid uid_ = boost::uuids::random_generator()();
        auto iter = uid_.begin();
        return (boost::uuids::to_string(uid_));
    }

    template
        <
        typename T,
	typename R
        >
        static
        R
     chkCoder(char* begin, char* end, size_t _blockIndex)
    {

       // LOG("chkCoder<T,R>(char* begin, char* end, size_t _blockIndex)");
       // _LOG::out_s << "input type = " << typeid(T).name() << std::endl;
       // LOG(_LOG::out_s);
       // getc(stdin);
        const R& result = chkCoderFunc(reinterpret_cast<T *>(begin), reinterpret_cast<T *>(end), _blockIndex);
       // _LOG::out_s << "!!!!!!!!!!!!!!!!!!!!!!result=" << result << std::endl;
       // LOG(_LOG::out_s);
        return (result);
    }

    static
    size_t
    chkCoderFunc(unsigned long *_tbegin,unsigned long *_tend,size_t _blockIndex)
    {
//	    LOG("sz_t chkcoder(ul*,ul*,sz_t)");
        size_t corrector = 0;
        size_t result = 0;
        size_t t_count = (_tend - _tbegin);
        for (size_t i = 0, j = (_tend - _tbegin); true; ++i, --j)
        {
            if ((j <= i) || (j == 0))
                break;
            size_t part_i = (_tbegin[i] * ++i); --i;
            corrector += _tend[j] * j;
            result += (part_i ^ corrector) * ++_blockIndex;
        }
        return (result);
    }

    static
    size_t
    chkCoderFuncX(DWORD64* _tbegin, DWORD64* _tend, size_t _blockIndex)
    {
     //   LOG("sz_t chkcoder(DW64*,DW64*,sz_t)");
        size_t corrector = 0;
        size_t result = 0;
        size_t t_count = _tend - _tbegin;
        for (size_t i = 0, j = (_tend - _tbegin); true; ++i, --j)
        {
            if ((j <= i) || (j == 0))
                break;
            size_t part_i = (_tbegin[i] * ++i); --i;
            corrector += _tend[j] * j;
            result += (part_i ^ corrector) * ++_blockIndex;
        }
        return (result);
    }

    static
    types::ul_long
    chkCoderFunc(types::DWORD64* _tbegin,types::DWORD64* _tend, size_t _blockIndex)
    {
      //  LOG("ull chkcoder(DW64*,DW63*,sz_t)");
        size_t corrector = 0;
        ul_long result = 0;
        size_t t_count = _tend - _tbegin;
        for (size_t i = 0, j = t_count; true; ++i, --j)
        {
            if (j <= i) //|| (j == 0))
                break;

            switch (i % 4)
            {
                case 0:
                    if (j <= i) //|| (j == 0))
                        break;
                    result += ((_tbegin[i] ^ _tend[j]) ^ (i ^ j)) ^ _blockIndex;
                    ++i; j -= i;              

                case 2:
                    if (j <= i) //|| (j == 0))
                        break;
                    result += ((_tbegin[i] ^ _tend[j]) ^ (i ^ j)) ^ _blockIndex;
                    ++i; j -= i;
                case 1:
                    if (j <= i) //|| (j == 0))
                        break;
                    result += ((_tbegin[i] ^ _tend[j]) ^ (i ^ j)) ^ _blockIndex;
                    ++i; j -= i;
                    
                case 3:
                    if (j <= i) //|| (j == 0))
                        break;
                    result += ((_tbegin[i] ^ _tend[j]) ^ (i ^ j)) ^ _blockIndex;
                    ++i; j -= i;
                case 4:
                    if (j <= i) //|| (j == 0))
                        break;
                    result += ((_tbegin[i] ^ _tend[j]) ^ (i ^ j)) ^ _blockIndex;
                    ++i; j -= i;
            }

        }
        return (result);
    }


    static
    types::ul_long
    chkCoderFunc(types::ui_int *_tbegin, types::ui_int *_tend, size_t _blockIndex)
    {
	//    LOG("ull chkcoder(ui*,ui*,sz_t)");
        auto test = sizeof(unsigned int);
        size_t corrector = 0;
        types::ul_long result = 0;
	    size_t t_count = (_tend - _tbegin);
        for (size_t i = 0; i < t_count ; ++i)
        {
	        types::ul_long part_i = (_tbegin[i] ^ i);;
            result += part_i;
        }
       // _LOG::out_s << "sizeof(result) = " << sizeof(result) << " result =" << result << std::endl;
       // LOG(_LOG::out_s);
        return (result);
    }

    //struct array_
    template <
        size_t size_
    >
    struct Buffer
    {
       char data[size_];
       types::ul_long size;
       types::ul_long chk;
    };

  

    template
        <
        typename T,
        short int count
        >
    struct u
    {     
        union _UBuffer
        {
            Buffer< sizeof(T) * count > buff;
            char data[ sizeof(buff) ];
        };    
    };
};
#endif