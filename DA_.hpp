#include "typedef.hpp"
#ifndef __DA__
#define __DA__
namespace DA
{
    enum { EMPTY_BUFFER_SIZE = 1, NORMAL_BUFFER_SIZE = 1000000, _GUID_SIZE = 40 };
    enum FILLERS
    {
        FILESIZE_INCREASE           = 2001,
        FILESIZE_REDUCTION          = 2002,
        FILLER_FILESIZE_REPLACEMENT = 2003,
        FILLER_FILESIZE_NO_CHANGED  = 2004,
        FILLER_FILESIZE_CREATED     = 2005,
        FILLER_FILESIZE_INCREASE    = 2006,
        FILLER_IS_ZERO_FILE         = 2007
    };
    enum ERRORS
    {
        UNCKNOWN_ERROR = 2202
    };
 
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
       mainDescNorm(const mainDesc_s& md):fileGuid(md.fileGuid),tokenFile(md.tokenFile),pathFile(md.pathFile),
       sessionId(md.sessionId),fileName(md.fileName),typeOf(md.typeOf)
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
    };
    struct null_
    {
    };

}
 #endif