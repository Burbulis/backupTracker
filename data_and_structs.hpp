#ifndef DATA_HPP
#include "typedef.hpp"
#include <stdio.h>
#include <string>
#include <cstring> 
#define DATA_HPP

    #define __SERIALISER_ON__

    typedef unsigned long long ul_long;
    typedef unsigned long ulong;
    typedef unsigned int  ui_int;

    enum
    {
        QUERY_TYPE_SIZE = 1,
        EMPTY_BUFFER_SIZE = QUERY_TYPE_SIZE,
        CREATE_BACKUP_BASE=1000000,
        _GUID_SIZE = 40,
        FA_COUNT_PER_PACKET_6 = 6,
        FA_COUNT_PER_PACKET_10 = 10,
        FA_COUNT_PER_PACKET_12 = 12,
        FA_COUNT_PER_PACKET_1000 =1000, 
        FA_COUNT_PER_PACKET_1200 = 1200,
        FA_COUNT_PER_PACKET_1400 = 1400,
        FA_COUNT_PER_PACKET_1600 = 1600,
        FA_COUNT_PER_PACKET_1800 = 1800,
        FA_COUNT_PER_PACKET_2000 = 2000,
        FA_COUNT_PER_PACKET_2200 = 2200,
        FA_COUNT_PER_PACKET_4000 = 14000,
      //  FA_COUNT_PER_PACKET_4000 = 4000,
        CREATE_NEW_RECORDS =1001,
        GET_RECORDS_FOR_CHECKING = 1002,
        UPDATE_RECORDS_COUNT6 = 1003,
        UPDATE_RECORDS_COUNT12 = 1004,
        QUERY_FILES_AND_PATHS =1005,
        QUERY_LAYERS       =1006 ,
        ADD_NEW_RECORD = 1007
    };

    enum FILLERS
    {
        FILESIZE_INCREASE           = 2001,
        FILESIZE_REDUCTION          = 2002,
        FILLER_FILESIZE_REPLACEMENT = 2003,
        FILLER_FILESIZE_NO_CHANGED  = 2004,
        FILLER_FILESIZE_CREATED     = 2005,
        FILLER_FILESIZE_INCREASE    = 2006
    };

    enum ERRORS
    {
        UNCKNOWN_ERROR = 2202
    };


    template
    <
        size_t INTERNAL_BUFF_SZ
    >
    struct tfileAttrib_
    {
        enum { BUFFER_SIZE = INTERNAL_BUFF_SZ,IS_QUERY = (BUFFER_SIZE == QUERY_TYPE_SIZE) };
        char begin[6];
        char layerGuid[_GUID_SIZE];
        char blockGuid[_GUID_SIZE];
        char bucketGuid[_GUID_SIZE];
        char buffer[BUFFER_SIZE];
        ul_long chk;
        ul_long index;
        ul_long bucket;
        ul_long size;
        char end[6];
    };


    //enum{BEGIN = 1,LAYER_GUID,BLOCK_GUID,CHK,INDEX,STATUS,SIZE,END};
    struct tfileAttrib_s
    {
        std::string begin;
        std::string layerGuid;
        std::string blockGuid;
        std::string bucketGuid;
        std::string buffer;
        ul_long chk;
        ul_long index;
        ul_long bucket;
        ul_long size;
        char end;
    };





    struct fileAttrib_
    {
         enum { BUFFER_SIZE = 0};
        char begin[6];
        char layerGuid[_GUID_SIZE];
        char blockGuid[_GUID_SIZE];
        char bucketGuid[_GUID_SIZE];
        ul_long chk;
        ul_long size;
        ul_long index;
        ul_long bucket;
        ul_long chk_bucket;
        char end[6];
    };

    typedef fileAttrib_ _fileAttrib; 
    typedef tfileAttrib_< CREATE_BACKUP_BASE > backupFileAttrib;
    typedef tfileAttrib_< QUERY_TYPE_SIZE > fileAttrib;
   
    
/*
main description about any request 


#pragma pack(push, 1)//- выключить
    struct mainDesc
    {
        char tokenFile[_GUID_SIZE];
        char fileGuid [_GUID_SIZE];
        char sessionId[_GUID_SIZE];
        types::ul_long typeOf;
        types::ul_long size;
        types::ul_long chk;
        char ftCreationTime  [TIME_SZ];
        char ftLastAccessTime[TIME_SZ];
        char ftLastWriteTime [TIME_SZ];
        char pathFile[MAX_P];
        char fileName[MAX_P];
    };
#pragma pack(pop)

 */
    enum { MAX_P = 800 ,TIME_SZ = 22};
    #pragma pack(push, 1)
    struct mainDesc
    {

        /// @brief do not change order fields without without urgent need! 
        //This field may be first only! It is strong rule! for detect this block in the any transfer buffers!
        char tokenFile[_GUID_SIZE];  
        /// @brief file Guid identify any file from system_ids table.
        char fileGuid [_GUID_SIZE];

        /// @brief reserved
        char sessionId[_GUID_SIZE];
        /// @brief type of request 
        //CREATE_NEW_RECORDS =1001,
        //GET_RECORDS_FOR_CHECKING = 1002,
        //UPDATE_RECORDS_COUNT6 = 1003,
        //UPDATE_RECORDS_COUNT12 = 1004,
        //QUERY_LAYERS       =1005 
        types::ul_long typeOf;
        /// @brief size of Request Buffer
        types::ul_long size;
        /// @brief checkSum of buffer request if it needed.
        types::ul_long chk;       
        //file time variables.
        char ftCreationTime  [TIME_SZ];
        char ftLastAccessTime[TIME_SZ];
        char ftLastWriteTime [TIME_SZ];
        /// @brief file path as utf16 escape sequence
        char pathFile[MAX_P];
        /// @brief file name as utf16 escape sequence
        char fileName[MAX_P];
    };

    struct normalMainDesc
    {

        /// @brief do not change order fields without without urgent need! 
        //This field may be first only! It is strong rule! for detect this block in the any transfer buffers!
        std::string tokenFile;  
        /// @brief file Guid identify any file from system_ids table.
        std::string fileGuid;

        /// @brief reserved
        std::string sessionId;
        /// @brief type of request 
        //CREATE_NEW_RECORDS =1001,
        //GET_RECORDS_FOR_CHECKING = 1002,
        //UPDATE_RECORDS_COUNT6 = 1003,
        //UPDATE_RECORDS_COUNT12 = 1004,
        //QUERY_LAYERS       =1005 
        types::ul_long typeOf;
        /// @brief size of Request Buffer
        types::ul_long size;
        /// @brief checkSum of buffer request if it needed.
        types::ul_long chk;       
        //file time variables.
        std::string ftCreationTime  ;
        std::string ftLastAccessTime;
        std::string ftLastWriteTime ;
        /// @brief file path as utf16 escape sequence
        std::string pathFile;
        /// @brief file name as utf16 escape sequence
        std::string fileName;
    };





#pragma pack(pop)

    struct baseFileDesc
    {
        baseFileDesc(std::string fileGuid_,std::string pathFile_,std::string fileName_,types::ul_long _fileSize):fileSize(_fileSize)
        {
            if (fileGuid_.length() < _GUID_SIZE)
            {
                memset(fileGuid,0,_GUID_SIZE);
                strcpy(&fileGuid[0],fileGuid_.c_str());  
            }    
            if ((pathFile_.length() < MAX_P) &&(fileName_.length() < MAX_P))
            {
                memset(pathFile,0,_GUID_SIZE);
                strcpy(&pathFile[0],pathFile_.c_str());  
                memset(fileName,0,_GUID_SIZE);
                strcpy(&fileName[0],fileName_.c_str());                 
            }
        }
        /// @brief normal GUID 36 bytes len. 
        char fileGuid[_GUID_SIZE];
        /// @brief file path as utf16 escape sequence
        char pathFile[MAX_P];
        /// @brief file name as utf16 escape sequence
        char fileName[MAX_P];
        types::ul_long fileSize;
    
       enum{
            SIZE_OF_BLOCKS = sizeof(fileGuid) + sizeof(pathFile) + sizeof(fileName) + sizeof(fileSize)
        }; 
    };

  #pragma pack(push, 1)
      template 
    <
    typename T,
    ulong count
    >
    struct s_buffer
    {
        enum{ T_BUFF_SIZE = T::BUFFER_SIZE, SIZE_OF_BLOCKS = (sizeof(T) * count) + sizeof(mainDesc) ,_GUID_SIZE = 40 };
        mainDesc md;
        union
        {
            T t[count];
            char data[SIZE_OF_BLOCKS];
        };
      
    };
#pragma pack(pop)

    template
        <
        typename T,
        short int count
        >
    struct metaUnion
    {  
        enum {
             TBUFF_SIZE = s_buffer<T,count>::T_BUFF_SIZE, SIZE_OF_BLOCKS = s_buffer<T,count>::SIZE_OF_BLOCKS,BUFFER_SIZE = sizeof(s_buffer<T,count>),_GUID_SIZE = s_buffer<T,count>::_GUID_SIZE
        };
        union 
        {
            s_buffer<T,count> buff;
            char data[BUFFER_SIZE];
        };

          typedef T CURRENT_TYPE;
    };

    template
    <
        typename T
    >
    struct metaUnionSrp
    {
        enum{SIZEOF = sizeof(T)};
        union
        {
            T t;
            char data[SIZEOF];
        };

    };


   struct null_
   {
        enum{BUFFER_SIZE = 1};
   };

   struct selectReqPRocessed
   {
      char token[ _GUID_SIZE];
      types::ul_long countOfRecords;
      types::ul_long sizeOfReq;  
      enum{BUFFER_SIZE = sizeof(countOfRecords) + sizeof(sizeOfReq)};
   }; 


typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_6>   bufferType_6;
typedef metaUnion<selectReqPRocessed,1> reqProcessed;
typedef metaUnion<fileAttrib,FA_COUNT_PER_PACKET_12>  bufferType_12;
typedef metaUnion<backupFileAttrib,FA_COUNT_PER_PACKET_12>  writeBufferType_12;
typedef metaUnion<backupFileAttrib,FA_COUNT_PER_PACKET_6>  writeBufferType_6;
typedef metaUnion<null_, EMPTY_BUFFER_SIZE>  headerType;

//typedef metaUnion<backupFileAttrib,FA_COUNT_PER_PACKET_12> newLayerbufferType_12;
//typedef metaUnionTest<fileAttrib,FA_COUNT_PER_PACKET_12> bufferType_12Tst;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_1000>  bufferType_1000;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_1200> bufferType_1200;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_1400> bufferType_1400;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_1600> bufferType_1600;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_1800> bufferType_1800;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_2000> bufferType_2000;
typedef s_buffer<fileAttrib,FA_COUNT_PER_PACKET_2200> bufferType_2200;
typedef s_buffer<_fileAttrib,FA_COUNT_PER_PACKET_4000> bufferType_4000;
typedef s_buffer<_fileAttrib,FA_COUNT_PER_PACKET_10>   bufferType_10;

#endif