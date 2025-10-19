#include "typedef.hpp"
#ifndef __DA__
#define __DA__
namespace DA
{
    enum { EMPTY_BUFFER_SIZE = 1, NORMAL_BUFFER_SIZE = 1000000, _GUID_SIZE = 40 };

    typedef unsigned long long DWORD64;
    typedef unsigned long long ul_long;
    typedef unsigned long ulong;
    typedef unsigned int  ui_int;


    enum
    {
        CREATE_NEW_RECORDS = 1001,
        GET_RECORDS_FOR_CHECKING = 1002,
        UPDATE_RECORDS_COUNT6 = 1003,
        UPDATE_RECORDS_COUNT12 = 1004,
        QUERY_FILES_AND_PATHS =1005,
        QUERY_LAYERS = 1006,
        ADD_NEW_RECORD = 1007
    };


    template
        <
        size_t BUFFER_SIZE_
        >

        struct tdataAttrib
    {
        enum {
            INTERNAL_BUFFER_SIZE = BUFFER_SIZE_

        };
        char begin[6];
        char layerGuid[_GUID_SIZE];
        char blockGuid[_GUID_SIZE];
        char bucketGuid[_GUID_SIZE];
        char buffer[INTERNAL_BUFFER_SIZE];
        ul_long chk;
        ul_long index;
        ul_long bucket;
        char end[6];
    };


    struct tdataAttrib_s
    {
        std::string begin;
        std::string layerGuid;
        std::string blockGuid;
        std::vector<uint8_t> buffer;
        ul_long chk;
        ul_long index;
        ul_long status;
        std::string end;
    };

    struct dataAttrib
    {
        char begin[6];
        char layerGuid[_GUID_SIZE];
        char blockGuid[_GUID_SIZE];
        char bucketGuid[_GUID_SIZE];
        ul_long chk;
        ul_long index;
        ul_long bucket;
        ul_long chk_bucket;
        char end[6];
    };

    template
        <
        typename T,
        size_t count
        >
        class DAtype
    {
        static T* dataBuffer;
    public:
        enum {
            FA_COUNT_PER_PACKET_6 = 6,
            FA_COUNT_PER_PACKET_12 = 12,
            BLOCKS_PER_BUCKETS = count
        };
        static
            T*
            create()
        {
            dataBuffer = new T[BLOCKS_PER_BUCKETS + 1];
            RtlZeroMemory(dataBuffer, sizeof(T) *( BLOCKS_PER_BUCKETS + 1) );
            return (&dataBuffer[0]);
        }

        static
            void
            reinit(void)
        {
            RtlZeroMemory(dataBuffer, sizeof(T) * count);
        }

        static
            T*
            attrinit(
                std::string  _layerGuid,
                size_t index,
                std::string _blockGuid,
                std::string _bucketGuid,
                ul_long bucketIndex,
                size_t _chk,
                size_t _index,
                char* _buffer,
                bool isFinalElement
            )
        {
            auto test = T::INTERNAL_BUFFER_SIZE;
            if (index > BLOCKS_PER_BUCKETS)
                return NULL;
            strcpy_s(dataBuffer[index].begin, "ABCD_");
            strcpy_s(dataBuffer[index].end, (isFinalElement) ? "_END" : "_DCBA");
            dataBuffer[index].chk = _chk;
            dataBuffer[index].index = _index;
            dataBuffer[index].bucket = bucketIndex;
            ZeroMemory(dataBuffer[index].blockGuid, _GUID_SIZE);
            ZeroMemory(dataBuffer[index].layerGuid, _GUID_SIZE);
            ZeroMemory(dataBuffer[index].bucketGuid, _GUID_SIZE);
            memcpy(dataBuffer[index].blockGuid, _blockGuid.c_str(), _blockGuid.length());
            memcpy(dataBuffer[index].layerGuid, _layerGuid.c_str(), _layerGuid.length());
            memcpy(dataBuffer[index].bucketGuid, _bucketGuid.c_str(), _bucketGuid.length());
            if (NULL != _buffer)
                memcpy(dataBuffer[index].buffer, _buffer, T::INTERNAL_BUFFER_SIZE);
            else
                memset(dataBuffer[index].buffer,0, T::INTERNAL_BUFFER_SIZE);
            return (&dataBuffer[index]);
        }


        static
            void
            setByBuffer(char* buffer, size_t size)
        {
            memcpy(dataBuffer, buffer, size);
        }
        static
            T*
            getDAByIndex(size_t index)
        {
            return (&dataBuffer[index]);
        }

        void
            clearByIndex(size_t index)
        {
            ZeroMemory(&dataBuffer[index], sizeof(dataBuffer[index]));
        }

        typedef T  _DA;
        typedef T* _PDA;
        typedef std::vector< T > SEQ;
        typedef std::vector< _PDA > PSEQ;

    };

    template
        <
        typename T,
        size_t count
        >
        T* DAtype<T, count>::dataBuffer;


    struct DAT_LIST
    {
        DAT_LIST()
        {
            auto sz = sizeof(DAtype<tdataAttrib<EMPTY_BUFFER_SIZE>, 12>::_DA);
        }
        typedef DAtype<tdataAttrib<EMPTY_BUFFER_SIZE>, 12>  QDAT12;
        typedef DAtype<tdataAttrib<NORMAL_BUFFER_SIZE>, 12> DAT12;
        typedef DAtype<tdataAttrib<NORMAL_BUFFER_SIZE>, 6> DAT6;
        typedef DAtype<dataAttrib, 20000> DAT10000;

    };

    struct mainDesc
    {
        enum { MAX_P = 200 };
        char tokenFile[MAX_P];
        char pathFile[MAX_P];
        char fileName[MAX_P];
        char fileGuid[_GUID_SIZE];
        char sessionId[_GUID_SIZE];
        types::ul_long typeOf;
        types::ul_long size;
        types::ul_long chk;
    };

    struct mainDesc_s
    {
        std::string tokenFile;
        std::string pathFile;
        std::string fileName;
        std::string fileGuid;
        std::string sessionId;
        types::ul_long typeOf;
        types::ul_long size;
        types::ul_long chk;
    
    mainDesc_s(std::string _tokenFile,
        std::string _pathFile,
        std::string _fileName,
        std::string _fileGuid,
        std::string _sessionId,
        types::ul_long _typeOf,
        types::ul_long _size):
        tokenFile(_tokenFile),
        pathFile(_pathFile),
        fileName(_fileName),
        fileGuid(_fileGuid),
        sessionId(_sessionId),
        typeOf(_typeOf),
        size(_size){}
     };


    struct mainDescNorm
    {
       // CREATION_TIME,LAST_ACCESS_TIME,LAST_WRITE_TIME
        /// @brief 
        /// @param _fileGuid 
        /// @param _tokenFile 
        /// @param _sessionId 
        /// @param _pathFile 
        /// @param _fileName 
        /// @param _typeOf 
        /// @param _size 
        /// @param _chk 
        mainDescNorm(
        std::string _fileGuid,
        std::string _tokenFile,
        std::string _sessionId,
        std::string _pathFile,
        std::string _fileName, 
        types::ul_long _typeOf,
        types::ul_long _creationTime,
        types::ul_long _lastAccessTime,
        types::ul_long _lastWriteTime,
        std::vector<DA::tdataAttrib_s> _blocks
        ):
       fileGuid (_fileGuid),
       tokenFile(_tokenFile),
       sessionId(_sessionId),
       pathFile (_pathFile),
       fileName (_fileName),
       typeOf   (_typeOf),
       creationTime  (_creationTime),
       lastAccessTime(_lastAccessTime),
       lastWriteTime (_lastWriteTime),
       blocks   (_blocks)
       {}
        typedef  DA::tdataAttrib_s blocksElementType;
        typedef  const DA::tdataAttrib_s constblocksElementType;
        typedef  std::vector<blocksElementType> blocksType;
  
        std::string fileGuid;
        std::string tokenFile;
        std::string sessionId;
        std::string pathFile;
        std::string fileName; 
        types::ul_long typeOf;
        types::ul_long creationTime;
        types::ul_long lastAccessTime;
        types::ul_long lastWriteTime;
        blocksType blocks;
        // std::vector<uint8_t> buffer;
    };

    struct selectReqPRocessed
    {
        char tokenName[_GUID_SIZE];
        types::ul_long countOfRecords;
        types::ul_long sizeOfReq;
    };

    struct null_
    {
    };

    template <
        size_t size_
    >
    struct Buffer
    {
        enum { _GUID_SIZE = 40 };
        mainDesc md;
        char data[size_];
    };




    template
        <
        typename T,
        short int count
        >
        struct  metaUnion
    {
        enum {
            SIZE_OF = (sizeof(T) * count) + sizeof(mainDesc),
            BUFFER_SIZE = sizeof(Buffer< SIZE_OF >),
            _GUID_SIZE = Buffer< SIZE_OF >::_GUID_SIZE
        };
        union //_UBuffer
        {
            char data[BUFFER_SIZE];
            Buffer< SIZE_OF > buff;
        };
    };

    typedef  DA::metaUnion<DA::null_, EMPTY_BUFFER_SIZE>  headerType;
/*
template
<
    typename T,
     size_t RANGE_BEGIN,
     size_t RANGE_END,
     size_t RESULT
>
struct mem_mgr
{
    std::unique_ptr< DA::metaUnion<T, RESULT> > mu;
    mem_mgr(size_t sizeOf)
    {
       // buffer =  ? new[RESULT] : nullptr;
        mu = ((RANGE_BEGIN > sizeOf) && (sizeOf < RANGE_END))?std::make_unique< DA::metaUnion<T, RESULT> >():nullptr;
    }

    char* ptr()
    {
        return mu.get();
    }

    ~mem_mgr()
    {
        
    }
};


*/
}
 #endif